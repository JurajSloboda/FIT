-- cpu.vhd: Simple 8-bit CPU (BrainF*ck interpreter)
-- Copyright (C) 2019 Brno University of Technology,
--                    Faculty of Information Technology
-- Author(s): DOPLNIT
--

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity cpu is
 port (
   CLK   : in std_logic;  -- hodinovy signal
   RESET : in std_logic;  -- asynchronni reset procesoru
   EN    : in std_logic;  -- povoleni cinnosti procesoru
 
   -- synchronni pamet RAM
   DATA_ADDR  : out std_logic_vector(12 downto 0); -- adresa do pameti
   DATA_WDATA : out std_logic_vector(7 downto 0); -- mem[DATA_ADDR] <- DATA_WDATA pokud DATA_EN='1'
   DATA_RDATA : in std_logic_vector(7 downto 0);  -- DATA_RDATA <- ram[DATA_ADDR] pokud DATA_EN='1'
   DATA_RDWR  : out std_logic;                    -- cteni (0) / zapis (1)
   DATA_EN    : out std_logic;                    -- povoleni cinnosti
   
   -- vstupni port
   IN_DATA   : in std_logic_vector(7 downto 0);   -- IN_DATA <- stav klavesnice pokud IN_VLD='1' a IN_REQ='1'
   IN_VLD    : in std_logic;                      -- data platna
   IN_REQ    : out std_logic;                     -- pozadavek na vstup data
   
   -- vystupni port
   OUT_DATA : out  std_logic_vector(7 downto 0);  -- zapisovana data
   OUT_BUSY : in std_logic;                       -- LCD je zaneprazdnen (1), nelze zapisovat
   OUT_WE   : out std_logic                       -- LCD <- OUT_DATA pokud OUT_WE='1' a OUT_BUSY='0'
 );
end cpu;


-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of cpu is

	--register pre PC
	signal PcReg : std_logic_vector(12 downto 0); 
	signal PcInc : std_logic;
	signal PcDec : std_logic;
	signal PcClear : std_logic;

	--register pre Ptr
	signal PtrReg : std_logic_vector(12 downto 0); 
	signal PtrInc : std_logic; 
	signal PtrDec : std_logic; 
	signal PtrClear : std_logic;

	--register pre Cnt
	signal CntReg : std_logic_vector(11 downto 0);
	signal CntInc : std_logic; 
	signal CntDec : std_logic;
	signal CntClear : std_logic;

	--multiplexory, mux1 a mux2 sú 2-1 a mux3 je 4-1
	signal mux1 : std_logic;
	signal mux2 : std_logic;
	signal mux3 : std_logic_vector(1 downto 0);
	--signal mx2out : std_logic_vector(12 downto 0); 
	--signal Tmp : std_logic_vector(7 downto 0) := "00000000";

	--všetky stavy pre FSM (automat)
	type FsmState is (
		Start,
		Load,
		Decode,
		PtrIncrease,
		PtrDecrease,
		ValIncrease,ValIncrease1,ValIncrease2,
		ValDecrease,ValDecrease1,ValDecrease2,
		WhileStart,WhileStart1,WhileStart2,WhileStart3,WhileStart4,WhileStart5,WhileStart6,
		WhileEnd,WhileEnd1,WhileEnd2,WhileEnd3,WhileEnd4,WhileEnd5,WhileEnd6,
		PrintChar,PrintChar1,
		GetChar,GetChar1,
		SaveToTmp,SaveToTmp1,SaveToTmp2,
		GetFromTmp,GetFromTmp1,GetFromTmp2,GetFromTmp3,
		EndReturn,
		Other
	);

	--nastavenie počiatočného stavu
	signal FsmPState : FsmState := Start; 
	signal FsmNext : FsmState; 
begin

	--proces pre ovládanie pc registru
	PCcnt: process (CLK, RESET)
	begin
		if RESET = '1' then
			PcReg <= (others => '0');
		elsif CLK'event and CLK = '1' then
			if PcInc = '1' then
				PcReg <= PcReg + 1;
			elsif PcDec = '1' then
				PcReg <= PcReg - 1;
			elsif PcClear = '1' then
				PcReg <= (others => '0');
			end if;
		end if;
	end process;
	

	--proces pre ovládanie ptr registru
	Ptrcnt: process (CLK, RESET)
	begin
		if RESET = '1' then
			PtrReg <= "1000000000000";
		elsif CLK'event and CLK = '1' then
			if PtrInc = '1' then
				PtrReg <= PtrReg + 1;
			elsif PtrDec = '1' then
				PtrReg <= PtrReg - 1;
			elsif PtrClear = '1' then
				PtrReg <="1000000000000";
			end if;
		end if;
	end process;

	--proces pre ovládanie cnt registru
	Cntcnt: process (CLK, RESET)
	begin
		if RESET = '1' then
			CntReg <= (others => '0');
		elsif CLK'event and CLK = '1' then
			if CntInc = '1' then
				CntReg <= CntReg + 1;
			elsif CntDec = '1' then
				CntReg <= CntReg - 1;
			elsif CntClear = '1' then
				CntReg <= (others => '0');
			end if;
		end if;
	end process;

	--nastavenie out rdata na outdata 
	OUT_DATA <= DATA_RDATA;

	--proces na ovládanie multiplexorov mux1 a mux2
	mx12: process(CLK, mux1,mux2, PcReg, PtrReg)
	begin
		--mux1 v 0 ide z mux2, v 1 ide z PcReg
		case mux1 is
			when '0' => 
				--mux2 v 0 ide z PtrReg a v 1 ide z tmp adresy
				if mux2 = '0' then
					DATA_ADDR <= PtrReg;
				elsif mux2 = '1' then
					DATA_ADDR <= "1000000000000";
					
				end if ;
			when '1' =>
				DATA_ADDR <= PcReg;
			when others =>
		end case;
	end process;
	
	--multiplexor mux3, ktorého výstup ide na DATA_WDATA
	MXdatawrite: process (CLK, mux3,DATA_RDATA,IN_DATA)
	begin
		if CLK'event and CLK = '1' then
			case mux3 is
				--v 00 ide vstup z IN_DATA
				when "00" =>
					DATA_WDATA <= IN_DATA;
				--v 01 sa pričítava RDATA
				when "01" =>
					DATA_WDATA <= DATA_RDATA + 1;

				when "10" =>
				-- v 10 sa odčítava RDATA
					DATA_WDATA <= DATA_RDATA - 1;
				-- v 11 sa len RDATA prepisuje na WDATA
				when "11" =>
					DATA_WDATA <= DATA_RDATA;

				when others =>

			end case;
		end if;
	end process;
	
	--proces pre ovládanie FSM automatu
	FsmPStateproc: process (CLK, RESET, EN)
	begin
		if RESET = '1' then
			FsmPState <= Start;
		elsif CLK'event and CLK = '1' then
			if EN = '1' then
				FsmPState <= FsmNext;
			end if;
		end if;
	end process;
			
	--FSM automat 
	FsmNStateproc: process (FsmPState, OUT_BUSY, IN_VLD, CntReg, DATA_RDATA)
	begin
		
		DATA_EN <= '0';
		DATA_RDWR <= '0';
		mux2 <= '0';
		mux1 <= '0';
		mux3 <= "11";
		PtrDec <= '0';
		PtrInc <= '0';
		PcDec <= '0';
		PcInc <= '0';
		CntInc <= '0';
		CntDec <= '0';
		IN_REQ <= '0';
		OUT_WE <= '0';

		PcClear <= '0';
		PtrClear <= '0';
		CntClear <= '0';
		case FsmPState is
			--počiatočný stav pri resete
			when Start=>
				PcClear <= '1';
				PtrClear <= '1';
				CntClear <= '1';
				FsmNext <= Load;
			--načítanie znaku na RDATA
			when Load=>
				DATA_EN <= '1';
				DATA_RDWR <= '0';
				mux1 <= '1';
				mux2 <= '0';
				FsmNext <= Decode;
			--dekodovanie znaku
			when Decode=>
				case DATA_RDATA is
					when X"3E" =>
						FsmNext <= PtrIncrease; --done
					when X"3C" =>
						FsmNext <= PtrDecrease; --done
					when X"2B" =>
						FsmNext <= ValIncrease; --done
					when X"2D" =>
						FsmNext <= ValDecrease;--done
					when X"5B" =>
						FsmNext <= WhileStart; 
					when X"5D" =>
						FsmNext <= WhileEnd; 
					when X"2E" =>
						FsmNext <= PrintChar;--done
					when X"2C" =>
						FsmNext <= GetChar; --done
					when X"24" =>
						FsmNext <= SaveToTmp; --done
					when X"21" =>
						FsmNext <= GetFromTmp; --done
					when X"00" =>
						FsmNext <= EndReturn;
					when others =>
						FsmNext <= Other;
				end case;

			--zvýšenie hodnoty ukazateľu
			when PtrIncrease=>
				PtrInc <= '1';
				PtrDec <= '0';
				PcInc <= '1';
				PcDec <= '0';

				FsmNext <= Load;

			--zníženie hodnoty ukazateľu
			when PtrDecrease=>
				PtrInc <= '0';
				PtrDec <= '1';
				PcInc <= '1';
				PcDec <= '0';

				FsmNext <= Load;

			--zvýšenie hodnoty
			when ValIncrease=>
				DATA_EN <= '1';
				
				mux1 <= '0';
				mux2 <= '0';
				DATA_RDWR <= '0';

				FsmNext <= ValIncrease1;

			when ValIncrease1 =>
				
				
				mux3 <= "01"; 

				FsmNext <= ValIncrease2;
			when ValIncrease2=>
				DATA_EN <= '1';
				DATA_RDWR <= '1'; 
				mux1 <= '0';
				mux2 <= '0';
				PcInc <= '1';
				PcDec <= '0';
				FsmNext <= Load;

			when ValDecrease=>
				DATA_EN <= '1';
				
				mux1 <= '0';
				mux2 <= '0';
				DATA_RDWR <= '0';
				FsmNext <= ValDecrease1;

			when ValDecrease1 =>
				
				mux3 <= "10";

				FsmNext <= ValDecrease2;

			when ValDecrease2 =>
				DATA_EN <= '1';
				DATA_RDWR <= '1';
				mux1 <= '0';
				mux2 <= '0';

				PcInc <= '1'; 
				PcDec <= '0';

				FsmNext <= Load;

			when GetChar=>
				IN_REQ <= '1';
				mux3 <= "00";
				FsmNext <= GetChar1;

			when GetChar1 =>
				if IN_VLD /= '1' then
					IN_REQ <= '1';
					mux3 <= "00";
					FsmNext <= GetChar1;
				else
					DATA_EN <= '1';
					DATA_RDWR <= '1';
					
					mux1 <= '0';
					mux2 <= '0';
					mux3 <= "00";
					PcInc <= '1'; 
					PcDec <= '0';

					FsmNext <= Load;
				end if;

			when PrintChar=>
				
				DATA_EN <= '1';
				
				mux1 <= '0';
				mux2 <= '0';
				DATA_RDWR <= '0';
				FsmNext <= PrintChar1;

			when PrintChar1 =>
			
				if OUT_BUSY = '1' then
					DATA_EN <= '1';
				
					mux1 <= '0';
					mux2 <= '0';
					DATA_RDWR <= '0';
					FsmNext <= PrintChar1;
				else
					OUT_WE <= '1'; 
					PcInc <= '1'; 
					PcDec <= '0';

					FsmNext <= Load;
				end if;

			when GetFromTmp =>
				mux2 <= '1';
				mux1 <= '0';
				FsmNext <= GetFromTmp1;
			when GetFromTmp1 =>

				DATA_EN <= '1';
				mux2 <= '1';
				mux1 <= '0';
				DATA_RDWR <= '0';
				
				
				FsmNext <= GetFromTmp2;
			when GetFromTmp2 =>
				
				mux2 <= '0';
				mux1 <= '0';
				mux3 <= "11";
				FsmNext <= GetFromTmp3;
			when GetFromTmp3 =>
				DATA_EN <= '1';
				mux2 <= '0';
				mux1 <= '0';
				DATA_RDWR <= '1';
				mux3 <= "11";
				
				PcInc <= '1';
				PcDec <= '0';

				
				--mux3 <= "11";

				FsmNext <= Load;


			when SaveToTmp =>
				DATA_EN <= '1';
				mux2 <= '0';
				mux1 <= '0';
				DATA_RDWR <= '0';
					

				FsmNext <= SaveToTmp1;
			when SaveToTmp1 =>
				
				mux2 <= '1';
				mux1 <= '0';
				mux3 <= "11";
				FsmNext <= SaveToTmp2;

			when  SaveToTmp2 =>
				
				DATA_EN <= '1';
				mux2 <= '1';
				mux1 <= '0';
				DATA_RDWR <= '1';
				mux3 <= "11";
				
				PcInc <= '1';
				PcDec <= '0';
				FsmNext <= Load;	

				
			when WhileStart =>
				PcInc <= '1';
				DATA_EN <= '1';
				mux1 <= '0';
				mux2 <= '0';
				DATA_RDWR <= '0';
				FsmNext <= WhileStart1;

			when WhileStart1 =>
				if (DATA_RDATA = "00000000") then
					FsmNext <= WhileStart2;
				else 
					FsmNext <= Load;
				end if;
				
			when WhileStart2 =>
				CntInc <= '1';
				FsmNext <= WhileStart3;

			--dasdsadsa
			when WhileStart3 =>
				if (CntReg = "000000000000") then
					FsmNext <= Load;
				else 
					FsmNext <= WhileStart4;
				end if;

			when WhileStart4 =>
				DATA_EN <= '1';
				mux1 <= '1';
				mux2 <= '0';
				DATA_RDWR <= '0';
				FsmNext <= WhileStart5;
				
			when WhileStart5 =>
				if (DATA_RDATA = X"5B")  then
					CntInc <= '1';
				elsif (DATA_RDATA =  X"5D") then
					CntDec <= '1';
				end if;
				FsmNext <= WhileStart6;

			when WhileStart6 =>
				PcInc <= '1';
				FsmNext <= WhileStart3;


				
			



			when WhileEnd =>
				DATA_EN <= '1';
				mux1 <= '0';
				mux2 <= '0';
				DATA_RDWR <= '0';
				FsmNext <= WhileEnd1;

			when WhileEnd1 =>
				
				if (DATA_RDATA = "00000000") then
					PcInc <= '1';
					FsmNext <= Load;
				else
					FsmNext <= WhileEnd2;
				end if;
				
			when WhileEnd2 =>
				CntInc <= '1';
				PcDec <= '1';
				
				FsmNext <= WhileEnd3;

			--dsadsadsa
			when WhileEnd3 =>
				if (CntReg = "000000000000") then
					FsmNext <= Load;
				else 
					FsmNext <= WhileEnd4;
				end if;
			
			when WhileEnd4 =>
				DATA_EN <= '1';
				mux1 <= '1';
				mux2 <= '0';
				DATA_RDWR <= '0';
				FsmNext <= WhileEnd5;	
					
			when WhileEnd5 =>
				if (DATA_RDATA = X"5B")  then
					CntDec <= '1';
				elsif (DATA_RDATA =  X"5D") then
					CntInc <= '1';
				end if;
				FsmNext <= WhileEnd6;
			when WhileEnd6 =>
				if (CntReg = "000000000000") then
					PcInc <= '1';
				else 
					PcDec <= '1';
				end if;
				FsmNext <= WhileEnd3;
					




			when EndReturn => 
				FsmNext <= EndReturn;
			when Other =>
					PcInc <= '1';
					PcDec <= '0';
					FsmNext <= Load;
			when others =>
					null;
		end case;
	end process;
 -- zde dopiste vlastni VHDL kod


 -- pri tvorbe kodu reflektujte rady ze cviceni INP, zejmena mejte na pameti, ze 
 --   - nelze z vice procesu ovladat stejny signal,
 --   - je vhodne mit jeden proces pro popis jedne hardwarove komponenty, protoze pak
 --   - u synchronnich komponent obsahuje sensitivity list pouze CLK a RESET a 
 --   - u kombinacnich komponent obsahuje sensitivity list vsechny ctene signaly.
 
end behavioral;
 
