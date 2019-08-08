package org.dkfsoft.model;

import lombok.Data;

@Data
public class Work {
    String title;
    String work_description;
    String work_name;
    String work_type;
    Integer work_type_id;
    Classificatory classificatory;
}
