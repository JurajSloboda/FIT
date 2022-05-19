package ija.ija2020.proj.store;

import java.time.LocalDate;
import java.util.ArrayList;
import java.util.Objects;

public class Goods {

    private String name;
    private ArrayList<GoodsItem> goodsList;

    public Goods(String name){
        this.name = name;
        goodsList = new ArrayList<GoodsItem>();
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        Goods goods = (Goods) o;
        return name.equals(goods.name);
    }

    @Override
    public int hashCode() {
        return Objects.hash(name);
    }

    public String getName() {
        return name;
    }

    public boolean addItem(GoodsItem goodsItem) {
        return goodsList.add(goodsItem);
    }

    public GoodsItem newItem(LocalDate localDate) {
        GoodsItem item = new GoodsItem(this, localDate);
        goodsList.add(item);
        return item;
    }

    public boolean remove(GoodsItem goodsItem) {
        return goodsList.remove(goodsItem);
    }

    public boolean empty() {
        return goodsList.isEmpty();
    }

    public int size() {
        return goodsList.size();
    }
}
