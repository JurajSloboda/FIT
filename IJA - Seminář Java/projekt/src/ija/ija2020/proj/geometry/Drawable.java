package ija.ija2020.proj.geometry;

import javafx.scene.shape.Shape;

import java.util.List;
import java.util.Map;

public interface Drawable {

    List<Shape> getGUI();
    boolean isHovered();
    Map<String, Integer> getContent();
}