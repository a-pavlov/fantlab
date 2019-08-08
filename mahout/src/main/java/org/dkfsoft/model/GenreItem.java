package org.dkfsoft.model;

import lombok.Data;

@Data
public class GenreItem {
    Integer genre_id;
    String label;
    Float percent;
    Integer votes;
}
