package ija.ija2020.proj;

import ija.ija2020.proj.store.GoodsOrder;
import ija.ija2020.proj.store.map.StoreNode;
import ija.ija2020.proj.vehicle.Cart;
import ija.ija2020.proj.calendar.Event;

import java.awt.*;
import java.util.*;
import java.util.List;

/**
 * Spawns and aggregates Carts. Accepts orders and assigns them to carts.
 * @see Cart
 */
public class CartController implements Observer {

    private int cartCapacity;
    private Queue<Cart> freeCarts = new LinkedList<>();
    private Queue<Cart> activeCarts = new LinkedList<>();

    private final int DEFAULT_CART_ITEMS_CAPACITY = 5;
    private final int DEFAULT_CART_SPEED = 1;
    private final StoreNode spawnPoint;

    public List<Shape> vehiclesgui = new ArrayList<>();

    public CartController(int cartCapacity, StoreNode spawnPoint) {
        this.cartCapacity = cartCapacity;
        this.spawnPoint = spawnPoint;
    }

    public int getCartCapacity() {
        return cartCapacity;
    }

    public void setCartCapacity(int cartCapacity) {
        this.cartCapacity = cartCapacity;
    }

    private Cart spawnCart(int capacity, int speed, StoreNode spawnPoint){
        //create new cart (it starts itself automatically)
        Cart cart = new Cart(this, capacity, speed, MainController.getInstance().getMap(), spawnPoint);
        MainController.getInstance().registerCart(cart);

        return cart;
    }

    /**
     * Accepts order if there are carts available
     * @param order order to assign to a cart
     * @return The cart that was assigned to fullfil this order, or null if there is no cart available
     */
    public Cart acceptOrder(GoodsOrder order){
        Cart cart = this.freeCarts.poll();
        if(cart == null) {
            if (this.cartCapacity - this.activeCarts.size() > 0){
                cart = spawnCart(DEFAULT_CART_ITEMS_CAPACITY, DEFAULT_CART_SPEED, spawnPoint);
            }else{
                return null;
            }
        }
        cart.acceptOrder(order);
        this.activeCarts.add(cart);
        return cart;
    }

    @Override
    public void update(Observable o, Object arg) {
        //Observes carts to see when they become free
        if (o instanceof Cart){
            Cart cart = (Cart) o;
            if(cart.isFree()){
                this.activeCarts.remove(cart);
                this.freeCarts.add(cart);
                MainController mainController = MainController.getInstance();
                mainController.getCalendar().insertEvent(new Event(mainController.getTime().plusSeconds(MainController.ORDER_ACCEPT_DELAY), 0, mainController::processOrderAction));
            }
        }
    }
}
