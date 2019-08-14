package org.dkfsoft.client;

import jdk.nashorn.internal.objects.annotations.Getter;
import org.dkfsoft.model.UserId;
import org.dkfsoft.model.Work;
import retrofit2.Response;
import retrofit2.http.GET;
import retrofit2.http.Header;
import retrofit2.http.Query;
import retrofit2.http.Url;
import rx.Observable;

public interface FantlabApiClient {

    @GET
    Observable<Response<Work>> getWork(@Url String url);

    @GET
    Observable<Response<UserId>> getUserByLogin(@Url String url, @Query("usersearch") String login);
}
