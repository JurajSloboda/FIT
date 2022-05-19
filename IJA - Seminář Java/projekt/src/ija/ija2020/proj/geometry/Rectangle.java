package ija.ija2020.proj.geometry;

/**
 * A rectangular area
 */
public class Rectangle {
    Position A; //bottom left
    Position B; //top right

    /**
     * Create a rectangele by specifying it's opposite corners
     * @param x1 Bottom left x coordinate
     * @param y1 Bottom left y coordinate
     * @param x2 Top right x coordinate
     * @param y2 Top right y coordinate
     */
    public Rectangle(int x1, int y1, int x2, int y2) {
        this.A = new Position(x1, y1);
        this.B = new Position(x2, y2);
    }

    /**
     * Create a rectangely by specifying it's opposite corners
     * @param t1 Bottom left corner
     * @param t2 Top right corner
     */
    public Rectangle(Targetable t1, Targetable t2){
        this.A = new Position(t1.getX(), t1.getY());
        this.B = new Position(t2.getX(), t2.getY());
    }

    public int getWidth(){
        return Math.abs(B.getX()-A.getX());
    }

    public int getHeight(){
        return Math.abs(B.getY()-A.getY());
    }

    public int getArea(){
        return getWidth() * getHeight();
    }

    public int getX1() {
        return A.getX();
    }

    public int getY1() {
        return A.getY();
    }

    public int getX2() {
        return B.getX();
    }

    public int getY2() {
        return B.getY();
    }

    public Targetable getBottomLeft(){ return A; }

    public Targetable getTopRight(){ return B; }

    public Targetable getBottomRight(){ return new Position(A.getX(), B.getY()); }

    public Targetable getTopLeft(){ return new Position(A.getY(), B.getX()); }

    /**
     * Computes the smallest distance to the target from a corner of this area;
     * @param target Target
     * @return Distance to the target
     */
    public int distance(Targetable target){
        int bl = this.getBottomLeft().distance(target);
        int br = this.getBottomRight().distance(target);
        int tr = this.getTopRight().distance(target);
        int tl = this.getTopLeft().distance(target);
        return Math.min(Math.min(bl, br), Math.min(tr, tl));
    }

    /**
     * Finds the corner of this area that is the closest to the target
     * @param from Target from which to compute distance
     * @return Corner of this area closest to the target (from)
     */
    public Targetable getClosestCorner(Targetable from){
        Targetable closest = this.getBottomLeft();
        if (this.getBottomRight().distance(from) < closest.distance(from)){
            closest = this.getBottomRight();
        }
        if(this.getTopRight().distance(from) < closest.distance(from)){
            closest = this.getTopRight();
        }
        if(this.getTopLeft().distance(from) < closest.distance(from)){
            closest = this.getTopLeft();
        }
        return closest;
    }
}
