package org.dkfsoft.model;

import lombok.AllArgsConstructor;
import lombok.Data;

@Data
@AllArgsConstructor
public class WorkMark {
    private Long workId;
    private Float markLL;
    private Float markCB;
    private String name;
    private String description;
    private Integer year;
    private String authors;
    private String mainGenre;

}
