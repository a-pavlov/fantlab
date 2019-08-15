package org.dkfsoft.model;

import lombok.AllArgsConstructor;
import lombok.Data;

import java.util.List;

@Data
@AllArgsConstructor
public class WorkMarkResponse {
    Long userId;
    List<WorkMark> workMarks;
}
