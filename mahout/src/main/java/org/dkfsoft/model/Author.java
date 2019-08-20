package org.dkfsoft.model;

import lombok.Data;

@Data
public class Author {
    Integer id;
    Integer is_opened;
    String name;
    String name_orig;
    String type;
}
