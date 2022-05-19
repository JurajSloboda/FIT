package ija.ija2020.proj.store;

import ija.ija2020.proj.vehicle.Cart;

import java.time.LocalDate;

public class GoodsItem {

    private LocalDate loadingDate;
    private Goods goodsType;
    private Stockable location;

    public GoodsItem(Goods goodsType, LocalDate loadingDate){
        this.loadingDate = loadingDate;
        this.goodsType = goodsType;
    }

    public void moveToLocation(Stockable destination) throws Stockable.StockableCapacityExceededException {
        if (location != null){
            this.location.takeItem(this, destination);
        }else{
            location.stockItem(this);
        }
        this.location = location;
    }

    public Goods getGoodsType() {
        return goodsType;
    }

    public Stockable getLocation(){
        return this.location;
    }

    public boolean sell() {
        return this.goodsType.remove(this);
    }
}
