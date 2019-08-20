package org.dkfsoft.model;

import lombok.Data;
import lombok.EqualsAndHashCode;

import java.util.List;
import java.util.stream.Collectors;

@Data
@EqualsAndHashCode(exclude = "title,work_description,work_name,work_type,work_type_id,work_year,classificatory")
public class Work {
    Long    work_id;
    String  title;
    String  work_description;
    String  work_name;
    String  work_type;
    Integer work_type_id;
    Integer work_year;
    Classificatory classificatory;
    List<Author> authors;

    public boolean isFantastic() {
        return isGenre(1);
    }

    public boolean isGenre(int genre) {
        return genre == 0 || (classificatory != null &&
                classificatory.getGenre_group() != null &&
                classificatory.getGenre_group().stream().flatMap(x -> x.genre.stream().map(y -> y.genre_id)).anyMatch(x -> x.equals(genre)));
    }

    public String getMainGenre() {
        return (classificatory != null && classificatory.getGenre_group() != null)?
                classificatory.getGenre_group().stream()
                .filter(x -> x.genre_group_id == 1)
                .map(x -> x.getGenre())
                .flatMap(x -> x.stream().map(y -> y.label))
                .collect(Collectors.joining(", ")): "N/A";
    }
}
