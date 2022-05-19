package ija.ija2020.proj.vehicle;

import ija.ija2020.proj.CartController;
import ija.ija2020.proj.MainController;
import ija.ija2020.proj.calendar.Event;
import ija.ija2020.proj.geometry.Drawable;
import ija.ija2020.proj.geometry.Movable;
import ija.ija2020.proj.geometry.Targetable;
import ija.ija2020.proj.map.GridNode;
import ija.ija2020.proj.store.*;
import ija.ija2020.proj.store.map.StoreMap;
import ija.ija2020.proj.store.map.StoreNode;
import javafx.event.EventHandler;
import javafx.scene.input.MouseEvent;
import javafx.scene.paint.Color;
import javafx.scene.shape.Line;
import javafx.scene.shape.Rectangle;
import javafx.scene.shape.Shape;
import javafx.util.Pair;

import java.time.LocalTime;
import java.util.*;

/**
 * Vehicle that can move through a store to fulfill orders
 * @see CartController
 * @see GoodsOrder
 */
public class Cart extends Observable implements Movable, Stockable, Drawable {
    private static final long START_UP_DELAY = 1;
    private final MainController mainController = MainController.getInstance();
    private final CartController parentController;

    private final StoreMap map;
    private int capacity; //capacity in units
    private int speed; //speed in units per second

    private GoodsOrder order;
    private LinkedList<GoodsItem> items;

    private Pair<OrderItem, GoodsShelf> closestPair = null;
    private Deque<GridNode> curPath = null;

    private StoreNode pos;
    //private StoreNode lastpos;
    private boolean isDroppingOff = false;
    private boolean isFree = true;

    private boolean hover = false;
    /**
     * Creates a new cart with a capacity and speed
     * Should be called from a CartController, otherwise the behaviour is undefined.
     * @see CartController
     * @param parentController The controller that handles this cart
     * @param capacity Number of items this cart can hold at once
     * @param speed Speed at which the cart moves in units per second
     * @param map Map through this cart will move
     * @param pos Node on which this cart will spawn
     */
    public Cart(CartController parentController, int capacity, int speed, StoreMap map, StoreNode pos) {
        this.parentController = parentController;
        this.addObserver(parentController);
        this.capacity = capacity;
        this.speed = speed;
        this.map = map;
        this.pos = pos;

        this.items = new LinkedList<>();
    }

    /**
     * Accepts the given order if it's not currently fulfilling another order
     * @param order the order to accept
     * @return True if the cart accepts the order, false if it's currently fulfilling another one
     */
    public boolean acceptOrder(GoodsOrder order){
        if (this.isFree) {
            this.isFree = false;
            this.order = order;
            order.setFulfillingVehicle(this);

            //schedule first event
            this.mainController.getCalendar().insertEvent(new Event(this.mainController.getTime().plusSeconds(this.START_UP_DELAY), 0, this::update));

            return true;
        }else{
            return false;
        }
    }

    /**
     * Is this car currently fulfilling an order
     * @return True if this cart is NOT currently fulfilling an order, False otherwise
     */
    public boolean isFree() {
        return isFree;
    }

    public boolean isFull(){
        return this.items.size() >= this.capacity;
    }

    public int getRemainingCapacity(){
        return this.capacity - this.items.size();
    }

    public float getSpeed() {
        return speed;
    }

    @Override
    public void moveTo(Targetable target) {
        this.pos.observers();
        this.pos = (StoreNode) this.map.getNode(target.getX(), target.getY());
        observers();
    }

    @Override
    public int getX() {
        return pos.getX();
    }

    @Override
    public int getY() {
        return pos.getY();
    }

    public GridNode getCurNode(){
        return pos;
    }

    @Override
    public int distance(Targetable target) {
        //Manhattan Distance
        return Math.abs(target.getX() - pos.getX()) + Math.abs(target.getY() - pos.getY());
    }

    private void plotPathToShelf(GoodsShelf shelf){
        //get the closest unobstructed node adjacent to the shelf
        GridNode nextNode = this.map.getNode(
                shelf.getArea().getClosestCorner(this.getCurNode())
        ).getClosestUnobstructedAdjacent(this.getCurNode());

        this.curPath = this.getCurNode().getPathToNodeOnGrid(this.map, nextNode);
    }

    /**
     * Plot path to the next (closest) item in our order
     */
    private void plotNewPath(){
        this.closestPair = this.order.getClosest(this.map);
        if(this.closestPair != null) {
            plotPathToShelf(this.closestPair.getValue());
        }else{
            this.curPath = null;
        }
    }

    private void plotPathToDropOffPoint(){
        this.isDroppingOff = true;
        this.closestPair = null;
        this.curPath = this.getCurNode().getPathToNodeOnGrid(this.map, this.map.getNode(this.order.getDropOffPoint().getX(), this.order.getDropOffPoint().getY()));
    }

    private boolean isCurPathObstructed() {
        Iterator<GridNode> iter = this.curPath.iterator();
        while(iter.hasNext()){
            if (iter.next().isObstructed()){
                return true;
            }
        }
        return false;
    }

    /**
     * Calendar event for moving the cart
     * @param time time now
     */
    public void update(LocalTime time) {
        System.out.println("T=" + time.toString() + " | Cart " + this.toString() + ": (" + this.getX() + ", " + this.getY() + ")");
        this.setChanged();
        this.notifyObservers();
        if(!this.isFree) {
            if (this.curPath != null) { //if we already have a path
                //check whether the path is still unobstructed
                while(this.isCurPathObstructed()){
                    this.plotNewPath();
                }

                GridNode nextNode = this.curPath.pollLast();
                if (nextNode != null) { //opposite should only happen if the order was empty or if the shelf is adjecent ot us
                    System.out.println(String.format("T=%s | Cart %s:(%d, %d) About to move to (%d, %d). Is it obstructed? %b",
                            time.toString(), this.toString(), this.getX(), this.getY(),
                            nextNode.getX(), nextNode.getY(), nextNode.isObstructed()
                    ));
                    this.moveTo(nextNode); //move to the next node on the path
                }
            } else {
                this.plotNewPath();
//                System.out.println(String.format("T=%s | Cart %s:(%d, %d) Ploting path to shelf %s",
//                        time.toString(), this.toString(), this.getX(), this.getY(),
//                        this.closestPair.getValue().getName()
//                ));
            }

            //check whether we made it to the end
            if (this.curPath.peekLast() == null) {
                if (this.isDroppingOff) {
                    //dump items
                    System.out.println("T=" + time.toString() + " | Cart " + this.toString() + ": (" + this.getX() + ", " + this.getY() + ") Dropping of items.");
                    DropOffPoint dropOffPoint = this.order.getDropOffPoint();
                    GoodsItem item;
                    while((item = this.items.poll()) != null){
                        dropOffPoint.stockItem(item);
                    }
                    this.isDroppingOff = false;

                    //check if order is fulfilled, or if this was an intermediary drop off
                    if(this.order.isGathered()) { //could prob be cashed when setting drop off point
                        //mark order as fulfilled
                        this.order.markAsFulfilled();
                        System.out.println(String.format("T=%s | Cart %s:(%d, %d) Order Fulfilled", time.toString(), this.toString(), this.getX(), this.getY()));

                        //reset cart
                        this.items = new LinkedList<>();
                        this.isDroppingOff = false;
                        this.isFree = true;
                        this.curPath = null;
                        this.setChanged();
                        this.notifyObservers();
                        return;
                    }else { //plot new path
                        this.plotNewPath();
                        System.out.println(String.format("T=%s | Cart %s:(%d, %d) Plotting path to shelf %s",
                                time.toString(), this.toString(), this.getX(), this.getY(),
                                this.closestPair.getValue().getName()
                        ));
                    }
                } else {
                    //gather from the shelf
                    System.out.println("T=" + time.toString() + " | Cart " + this.toString() + ": (" + this.getX() + ", " + this.getY() + ") Gathering from shelf");
                    closestPair.getKey().gatherFromShelf(closestPair.getValue(), this);
                }

                // check if we have gathered all the items or are full
                if (this.order.isGathered() || this.isFull()) {
                    //plot path to drop off point
                    System.out.println(String.format("T=%s | Cart %s:(%d, %d) Plotting path to drop off point. Order fulfilled: %s",
                            time.toString(), this.toString(), this.getX(), this.getY(),
                            this.order.isGathered()
                    ));
                    this.plotPathToDropOffPoint();
                }else {
                    //get next pair and path
                    this.plotNewPath();
                }
            }

            //schedule new event
            GridNode nextNode = this.curPath.peekLast();
            LocalTime t1;
            //if we haven't found any path, just try again with delay
            if(nextNode != null) {
                t1 = time.plusSeconds((long) (this.getCurNode().distance(nextNode) / this.getSpeed()));
                System.out.println(String.format("T=%s | Cart %s:(%d, %d) scheduling event to %s: Moving to (%d, %d)",
                        time.toString(), this.toString(), this.getX(), this.getY(),
                        t1.toString(), nextNode.getX(), nextNode.getY()
                ));
                System.out.println(String.format("DEBUG: curNode(%d, %d), nextNode(%d, %d) delta_t=%d; distance=%d; speed=%d",
                        this.getCurNode().getX(), this.getCurNode().getY(),
                        nextNode.getX(), nextNode.getY(),
                        (long)(this.getCurNode().distance(nextNode) / this.getSpeed()),
                        (long)this.getCurNode().distance(nextNode),
                        (long)this.getSpeed()
                        ));
            }
            else{
                t1 = time.plusSeconds(START_UP_DELAY);
                System.out.println(String.format("T=%s | Cart %s:(%d, %d) scheduling event to %s: Waiting",
                        time.toString(), this.toString(), this.getX(), this.getY(),
                        t1.toString()
                ));
            }
            this.mainController.getCalendar().insertEvent(new Event(t1, 0, this::update));
        }
    }

    @Override
    public void stockItem(GoodsItem item) throws StockableCapacityExceededException {
        if(this.items.size() + 1 > this.capacity){
            throw new StockableCapacityExceededException();
        }else{
            this.items.add(item);
        }
    }

    @Override
    public GoodsItem takeItem(GoodsItem item, Stockable destination) {
        if(this.items != null) {
            if (this.items.remove(item)){
                destination.stockItem(item);
                return item;
            }else{
                return null;
            }
        }
        return null;
    }

    @Override
    public GoodsItem takeAnyOfType(Goods type, Stockable destination) {
        if(this.items != null) {
            if (!this.items.isEmpty()) {
                GoodsItem result = this.items.remove(0);
                destination.stockItem(result);
                return result;
            }
        }
        return null;
    }

    private void observers(){
        this.setChanged();
        this.notifyObservers();
    }

    /**
     * Gets a dictionary containing info about the content of this cart
     * @return Content of this shelf in the format <Goods type name, number of items on this shelf>
     */

    @Override
    public Map<String, Integer> getContent(){
        Map<String, Integer> result = new HashMap<>();
        for (GoodsItem item : this.items){
            String name = item.getGoodsType().getName();
            if (result.containsKey(name)){
                result.replace(name, result.get(name)+1);
            }else {
                result.putIfAbsent(name, 1);
            }
        }
        return result;
    }


    @Override
    public boolean isHovered(){
        if (this.hover == true){
            return true;
        }
        return false;
    }

    private List<Shape> getPathGui(){

        Deque<GridNode> temppath = new ArrayDeque<>();
        List<Shape> result = new ArrayList<>();
        GridNode actnode = temppath.getFirst();
        GridNode nextnode;
        for(int i = 0; i < temppath.size() - 1; i++){
            System.out.println(actnode.getX() +  "   "+actnode.getY()  );
            nextnode = temppath.getFirst();
            Line line = new Line();
            line.setStartX(actnode.getX()*100);
            line.setStartY(actnode.getY()*100);
            line.setEndX(nextnode.getX()*100);
            line.setEndY(nextnode.getY()*100);
            line.setStrokeWidth(10);
            result.add(line);
            actnode = nextnode;
        }
        return result;
    }

    @Override
    public List<Shape> getGUI() {

        List<Shape> gui = new ArrayList<>();

        //lastpos.observers();

        Rectangle rect = new Rectangle(pos.getX()*100,  pos.getY()*100, 100, 100);
        if (hover == true){
            rect.setFill(Color.BLACK);
            //if(getPathGui() != null){
            //    gui.addAll(getPathGui());
            //}
        } else {
            rect.setFill(Color.RED);
        }

        rect.setStroke(Color.BLACK);
        rect.setStrokeWidth(2);
        rect.addEventHandler(MouseEvent.MOUSE_ENTERED,
                new EventHandler<MouseEvent>() {
                    @Override
                    public void handle(MouseEvent e) {
                        rect.setFill(Color.BLACK);
                        hover = true;
                        //if(getPathGui() != null){
                        //    gui.addAll(getPathGui());
                        //}

                        observers();
                    }
                });
        rect.addEventHandler(MouseEvent.MOUSE_EXITED,
                new EventHandler<MouseEvent>() {
                    @Override
                    public void handle(MouseEvent e) {
                        rect.setFill(Color.RED);
                        hover = false;
                        observers();
                    }
                });
        gui.add(rect);
        return gui;
    }
}
