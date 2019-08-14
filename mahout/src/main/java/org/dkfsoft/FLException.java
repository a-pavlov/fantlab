package org.dkfsoft;

import io.netty.handler.codec.http.HttpResponseStatus;
import lombok.Getter;

@Getter
public class FLException extends Exception {
    private HttpResponseStatus status;
    public FLException(HttpResponseStatus status, String message) {
        super(message);
        this.status = status;
    }
}
