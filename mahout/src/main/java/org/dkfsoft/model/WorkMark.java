package org.dkfsoft.model;

import lombok.AllArgsConstructor;
import lombok.Data;

@Data
@AllArgsConstructor
public class WorkMark {
    private Long workId;
    private Float mark;
    private String name;
    private String description;
}
