package ija.ija2020.proj.store;

import ija.ija2020.proj.geometry.Rectangle;

import java.util.*;

public class GoodsShelf implements Stockable{

    private final String name;
    private final Map<Goods, LinkedList<GoodsItem>> goodsDict;
    private final Rectangle area;

    public GoodsShelf(String name, Rectangle area) {
        this.name = name;
        this.goodsDict = new HashMap<Goods, LinkedList<GoodsItem>>();
        this.area = area;
    }

    public Rectangle getArea() {
        return area;
    }

    public String getName() {
        return name;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        GoodsShelf that = (GoodsShelf) o;
        return name.equals(that.name);
    }

    @Override
    public int hashCode() {
        return Objects.hash(name);
    }

    @Override
    public void stockItem(GoodsItem goodsItem) throws StockableCapacityExceededException {
        if(goodsDict.containsKey(goodsItem.getGoodsType())){
            goodsDict.get(goodsItem.getGoodsType()).add(goodsItem);
        }else{
            LinkedList<GoodsItem> newList = new LinkedList<GoodsItem>();
            newList.add(goodsItem);
            goodsDict.put(goodsItem.getGoodsType(), newList);
        }
    }

    public boolean containsGoods(Goods goods) {
        List goodsList = (List)goodsDict.get(goods);
        if (goodsList == null){
            return false;
        }else{
            return !goodsList.isEmpty();
        }
    }

    @Override
    public GoodsItem takeItem(GoodsItem item, Stockable destination) {
        List<GoodsItem> tmp = goodsDict.get(item.getGoodsType());
        if(tmp != null) {
            if (tmp.remove(item)){
                destination.stockItem(item);
                System.out.println("Removing item " + item.toString() + " of type " + item.getGoodsType().getName() + " from shelf " + this.name);
                System.out.println("There is " + goodsDict.size() + " pieces remaining of " + item.getGoodsType().getName() + " in shelf " + this.name);
                return item;
            }else{
                return null;
            }
        }
        return null;
    }

    @Override
    public GoodsItem takeAnyOfType(Goods goods, Stockable destination) {
        List<GoodsItem> tmp = goodsDict.get(goods);
        if(tmp != null) {
            if (!tmp.isEmpty()) {
                GoodsItem result = tmp.remove(0);
                destination.stockItem(result);
                System.out.println("Removing item " + result.toString() + " of type " + result.getGoodsType().getName() + " from shelf " + this.name);
                System.out.println("There is " + this.size(goods) + " pieces remaining of " + result.getGoodsType().getName() + " in shelf " + this.name);
                return result;
            }
        }
        return null;
    }

    /**
     * Gets a dictionary containing info about the content of this shelf
     * @return Content of this shelf in the format <Goods type name, number of items on this shelf>
     */
    public Map<String, Integer> getContent(){
        //System.out.println("Removing item " + result.toString() + " of type " + result.getGoodsType().getName() + " from shelf " + this.name);
        //System.out.println("There is " + this.size(goods) + " pieces remaining of " + result.getGoodsType().getName() + " in shelf " + this.name);
        Map<String, Integer> result = new HashMap<>();
        for (Map.Entry<Goods, LinkedList<GoodsItem>> entry : this.goodsDict.entrySet()){
            result.putIfAbsent(entry.getKey().getName(), entry.getValue().size());
        }
        return result;
    }

    /**
     * Counts number of goods of the specified type on this shelf
     * @param goods Goods Type to count
     * @return Number of items of type goods on this shelf
     */
    public int size(Goods goods) {
        List<GoodsItem> tmp = goodsDict.get(goods);
        if (tmp != null) {
            return tmp.size();
        }else{
            return 0;
        }
    }
}
