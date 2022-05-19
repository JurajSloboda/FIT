package ija.ija2020.proj.store;

import ija.ija2020.proj.geometry.Position;
import ija.ija2020.proj.geometry.Targetable;

public class DropOffPoint implements Stockable, Targetable {
    private final Position pos;

    public DropOffPoint(Position pos) {
        this.pos = pos;
    }

    @Override
    public void stockItem(GoodsItem item) throws StockableCapacityExceededException {
        //TODO: Take the item out of Goods
        return;
    }

    @Override
    public GoodsItem takeItem(GoodsItem item, Stockable destination) {
        return null;
    }

    @Override
    public GoodsItem takeAnyOfType(Goods type, Stockable destination) {
        return null;
    }

    @Override
    public int getX() {
        return this.pos.getX();
    }

    @Override
    public int getY() {
        return this.pos.getY();
    }

    @Override
    public int distance(Targetable target) {
        //Manhattan Distance
        return Math.abs(target.getX() - pos.getX()) + Math.abs(target.getY() - pos.getY());
    }
}
