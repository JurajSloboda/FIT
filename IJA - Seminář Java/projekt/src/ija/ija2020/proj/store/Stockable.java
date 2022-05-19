package ija.ija2020.proj.store;



/**
 * Represents an entity that can hold goods, i.e. a shelf or a cart
 * @see GoodsItem
 */
public interface Stockable {

    class StockableCapacityExceededException extends RuntimeException {
        public StockableCapacityExceededException(){}
        public StockableCapacityExceededException(String msg){super(msg);}
    }

    /**
     * Adds item to this stockable. The caller is responsible for removing the items from their own records
     * @param item
     * @throws StockableCapacityExceededException
     */
    void stockItem(GoodsItem item) throws StockableCapacityExceededException;

    /**
     * Removes a specific item from this entity and moves it to another location. If this entity doesn't hold the item, nothing is changed and null is returned
     * @param item Item identity to take
     * @param destination Entity that will hold the item if the operation is successful
     * @return The requested item, or null if this entity doesn't hold the item
     */
    GoodsItem takeItem(GoodsItem item, Stockable destination);

    /**
     * Removes any item of the specified goods type from this entity. If this entity doesn't hold any items of the specified type, nothing is changed and null is returned
     * @param type Goods type of the item to be taken
     * @param destination Entity that will hold the item if the operation is successful
     * @return A item of requested type, or null if this entity doesn't hold any items of the specified type
     */
    GoodsItem takeAnyOfType(Goods type, Stockable destination);
}
