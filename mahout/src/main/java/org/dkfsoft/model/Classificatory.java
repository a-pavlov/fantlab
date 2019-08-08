package org.dkfsoft.model;

import lombok.Data;

import java.util.List;

@Data
public class Classificatory {
    List<GenreGroup> genre_group;
    Integer total_count;
}
