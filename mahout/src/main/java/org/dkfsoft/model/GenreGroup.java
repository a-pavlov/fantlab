package org.dkfsoft.model;

import lombok.Data;

import java.util.List;

@Data
public class GenreGroup {
    List<Genre> genre;
    Integer genre_group_id;
    String label;
}
