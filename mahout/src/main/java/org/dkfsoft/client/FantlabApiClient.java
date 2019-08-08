package org.dkfsoft.client;

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
}
