package ija.ija2020.proj.store.map;

import ija.ija2020.proj.MainController;
import ija.ija2020.proj.geometry.Rectangle;
import ija.ija2020.proj.map.GridMap;
import ija.ija2020.proj.store.Goods;
import ija.ija2020.proj.store.GoodsShelf;

import java.util.*;

public class StoreMap extends GridMap {

    private Map<String, GoodsShelf> shelves = new HashMap<>();

    public StoreMap(int height, int width) {
        super(height, width);
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                StoreNode storeNode = new StoreNode(i, j, this);
                this.grid[i + j * width] = storeNode;
            }
        }
    }

    /**
     * Adds all shelves in a list to this map
     * @param shelves
     */
    public void addShelves(List<GoodsShelf> shelves){
        for(GoodsShelf shelf : shelves){
            this.addShelf(shelf);
        }
    }

    /**
     * Adds a shelf to this map if it's not a part of this map already.
     * Also registers this shelf with all nodes that this shelf would lie on
     * @param shelf
     */
    public void addShelf(GoodsShelf shelf){
        if (this.shelves.containsKey(shelf.getName())){
            throw new IllegalArgumentException("Shelf " + shelf.toString() + " already part of this map. Remove it first, or use the appropriate method for moving.");
        }
        //TODO: Check if this shelf is within bounds of the map
        //TODO: Check for overlap with another shelf
        this.shelves.put(shelf.getName(), shelf);
        Rectangle area = shelf.getArea();
        for (int i = area.getX1(); i <= area.getX2(); i++) {
            for (int j = area.getY1(); j <= area.getY2(); j++) {
                ((StoreNode) this.getNode(i, j)).setShelf(shelf);
            }
        }
    }

    /**
     * Finds all shelves that contain any items of the specified goods type
     * @param goods goods type
     * @return a list of shelves that contain the specified goods type
     */
    public List<GoodsShelf> getShelvesWithGoods(Goods goods){
        List<GoodsShelf> result = new LinkedList<>();
        for (GoodsShelf shelf : this.shelves.values()) {
            if (shelf.containsGoods(goods)) {
                result.add(shelf);
            }
        }

        return result;
    }

    public GoodsShelf getShelfByName(String name){
        return this.shelves.get(name);
    }


}
