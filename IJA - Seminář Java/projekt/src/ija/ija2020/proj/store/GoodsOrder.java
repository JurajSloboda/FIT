package ija.ija2020.proj.store;

import ija.ija2020.proj.geometry.Targetable;
import ija.ija2020.proj.store.map.StoreMap;
import ija.ija2020.proj.vehicle.Cart;
import javafx.util.Pair;

import java.util.*;

public class GoodsOrder extends Observable{

    private List<OrderItem> order;
    private int priority;
    private DropOffPoint dropOffPoint;
    private Cart fulfillingVehicle;
    private boolean isFulfilled = false;

    public GoodsOrder(int priority){
        this.priority = priority;
        this.dropOffPoint = dropOffPoint;
        this.order = new ArrayList<>();
    }

    public void setFulfillingVehicle(Cart fulfillingVehicle) {
        this.fulfillingVehicle = fulfillingVehicle;
    }

    public void setDropOffPoint(DropOffPoint dropOffPoint) {
        this.dropOffPoint = dropOffPoint;
    }

    public Cart getFulfillingVehicle() {
        return fulfillingVehicle;
    }

    public int getPriority() {
        return priority;
    }

    public DropOffPoint getDropOffPoint() {
        return dropOffPoint;
    }

    public void add(OrderItem article){
        order.add(article);
    }

    public void add(int amount, Goods goodType){
        order.add(new OrderItem(amount, goodType));
    }

    /**
     * Checks if we've already gathered all the items needed to fulfill this order
     * @return True if we've already gathered all the items needed to fulfill this order, false otherwise.
     * Will return true even if some of the items were already dropped off
     */
    public boolean isGathered(){
        for (OrderItem item : this.order) {
            if (item.getAmountRemaining() != 0) {//order item already fulfilled
                return false;
            }
        }
        return true;
    }

    public boolean isFulfilled() {
        return isFulfilled;
    }

    public void markAsFulfilled() {
        isFulfilled = true;
        fulfillingVehicle = null;
    }

    /**
     * Find the closest shelve that can be gathered from to continue fulfilling this order
     * @param map Store map to search
     * @return OrderItem and shelf that is closest to pos, or null if no shelves were found.
     */
    public Pair<OrderItem, GoodsShelf> getClosest(StoreMap map) {
        //Order the orders and shelves
        class StoreShelfDistanceComparator implements Comparator<GoodsShelf> {
            @Override
            public int compare(GoodsShelf o1, GoodsShelf o2) {
                if(o1.getArea().distance(fulfillingVehicle.getCurNode()) < o2.getArea().distance(fulfillingVehicle.getCurNode())){
                    return -1;
                }else if(o1.getArea().distance(fulfillingVehicle.getCurNode()) > o2.getArea().distance(fulfillingVehicle.getCurNode())){
                    return 1;
                }
                return 0;
            }
        }

        class OrderStoreShelfPairDistanceComparator implements Comparator<Pair<OrderItem, GoodsShelf>> {
            @Override
            public int compare(Pair<OrderItem, GoodsShelf> o1, Pair<OrderItem, GoodsShelf> o2) {
                if(o1.getValue().getArea().distance(fulfillingVehicle.getCurNode()) < o2.getValue().getArea().distance(fulfillingVehicle.getCurNode())){
                    return -1;
                }else if(o1.getValue().getArea().distance(fulfillingVehicle.getCurNode()) > o2.getValue().getArea().distance(fulfillingVehicle.getCurNode())){
                    return 1;
                }
                return 0;
            }
        }

        List<Pair<OrderItem, GoodsShelf>> shelves = new LinkedList<>();
        //for each item in order, find all shelves that hold that item and sort them by distance
        for (OrderItem item : this.order){
            if (item.getAmountRemaining() == 0){//order item already fulfilled
                continue;
            }
            List<GoodsShelf> tmp = map.getShelvesWithGoods(item.getGoodsType());

            tmp.sort(new StoreShelfDistanceComparator());
            //TODO: What happens if the order can't be fullfiled?
            shelves.add(new Pair<>(item, tmp.get(0))); //only save the closest shelve that holds this goods type
        }

        //sort the different shelves by distance
        shelves.sort(new OrderStoreShelfPairDistanceComparator());

        //Return the closest one
        try {
            return shelves.get(0);
        }catch(IndexOutOfBoundsException e){
            return null;
        }
    }

}
