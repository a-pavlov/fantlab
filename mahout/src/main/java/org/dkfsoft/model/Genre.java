package org.dkfsoft.model;

import lombok.Data;

import java.util.List;

@Data
public class Genre {
    List<GenreItem> genre;
    Integer genre_id;
    String label;
    Integer percent;
    Integer votes;
}
