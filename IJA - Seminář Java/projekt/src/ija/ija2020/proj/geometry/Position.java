package ija.ija2020.proj.geometry;

/**
 * Encodes a position in 2D space
 */
public class Position implements Targetable{

    private int x;
    private int y;

    public Position(int x, int y) {
        this.x = x;
        this.y = y;
    }

    @Override
    public int getX() {
        return x;
    }

    @Override
    public int getY() {
        return y;
    }

    @Override
    public int distance(Targetable target) {
        return (int) (Math.sqrt(Math.pow(Math.abs(this.getX() - target.getX()), 2) + Math.pow(Math.abs(this.getY() - target.getY()), 2)));
    }
}
