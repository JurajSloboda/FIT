package ija.ija2020.proj.geometry;

public interface Movable extends Targetable{

    /**
     * Move this object to target (Change it's position so that it matches the targets)
     * @param target Object whose position will be matched
     */
    public void moveTo(Targetable target);
}
