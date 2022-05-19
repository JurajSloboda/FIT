-- Autor reseni: SEM DOPLNTE VASE, JMENO, PRIJMENI A LOGIN

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use IEEE.std_logic_unsigned.all;

entity ledc8x8 is
port ( -- Sem doplnte popis rozhrani obvodu.
    SMCLK : in std_logic; 
	RESET : in std_logic; 
	ROW : out std_logic_vector(0 to 7); 
	LED : out std_logic_vector(0 to 7) 



);
end ledc8x8;

architecture main of ledc8x8 is


	signal ce : std_logic := '0';
	signal cnt : std_logic_vector(11 downto 0) := "000000000000"; --hlavný counter na SMCLK/256/8
	signal riadky : std_logic_vector(0 to 7) := "10000000";
	signal ledky : std_logic_vector(0 to 7) := "11111111";
	signal perm : std_logic := '0';
	signal off : std_logic := '1';
	signal cnt2 : std_logic_vector(9 downto 0) := "0000000000"; --counter na pol sekundy

begin
	
	-- čítač
	ceGenerator: process(SMCLK, RESET, cnt, cnt2, perm, off)
	begin
		if RESET = '1' then
			cnt <= "000000000000"; --vynulovanie hlavného counteru
			cnt2 <= "0000000000"; -- vynulovanie counteru na pol sekundy
		elsif SMCLK'event and SMCLK = '1' then
			if cnt = "111000010000" then --ak je čas na SMCLK/256 tak sa vynuluje hlavný counter a ide odznovu a inkrementuje counter na pol sekundy
				cnt <= "000000000000";
				cnt2 <= cnt2 + 1;
				if cnt2 = "1110000100" then --keď prejde pol sekundy tak sa menia perm a off a nuluje sa counter
					if perm = '0' then -- keď je perm v 0 tak ide na 1 a už sa nikdy nevráti späť aby sa nezmenilo off späť na 0 nikdy 
						perm <= '1';
						off <= '0';
					else 
						off <= '1';
					end if;
					cnt2 <= "0000000000"; -- vynulovanie counteru na po sekundy
				end if;
			end if;

			cnt <= cnt + 1;
		end if;
	end process ceGenerator;
	ce <= '1' when cnt = X"FF" else '0';

	start: process(riadky,off) -- proces na zapnutie potrebných LEDiek
	begin
		if off = '1' then --keď je off v 1 tak sú zapnuté potrebné LEDky
			case riadky is
				when "10000000" => ledky <= "00001111";
				when "01000000" => ledky <= "11101111";
				when "00100000" => ledky <= "11101111";
				when "00010000" => ledky <= "01101000";
				when "00001000" => ledky <= "10010111";
				when "00000100" => ledky <= "11111001";
				when "00000010" => ledky <= "11111110";
				when "00000001" => ledky <= "11110001";
				when others => ledky <= (others => '1');
			end case;
		else --keď je off v nule tak sú ledky vypnuté
			case riadky is
				when "10000000" => ledky <= "11111111";
				when "01000000" => ledky <= "11111111";
				when "00100000" => ledky <= "11111111";
				when "00010000" => ledky <= "11111111";
				when "00001000" => ledky <= "11111111";
				when "00000100" => ledky <= "11111111";
				when "00000010" => ledky <= "11111111";
				when "00000001" => ledky <= "11111111";
				when others => ledky <= (others => '1');
			end case;
		end if;
	end process start;
	LED <= ledky;

	rows_activate: process(SMCLK, RESET, ce ,off )
	begin
		if RESET = '1' then -- nulovanie hodnôt na počiatočné hodnoty
			riadky <= "10000000";
		elsif SMCLK'event and SMCLK = '1' then
			if ce = '1' then
				riadky <= riadky(7) & riadky(0 to 6) ;
			end if;
		end if;
	end process rows_activate;
	ROW <= riadky;

end main;




-- ISID: 75579
