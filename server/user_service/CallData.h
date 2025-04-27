//
// Created by Fan on 25-4-27.
//

#ifndef CALLDATA_H
#define CALLDATA_H

#include <grpcpp/grpcpp.h>

#include "auth/auth.grpc.pb.h"

class BaseCallData {
public:
    virtual void Proceed() = 0;  // 纯虚函数接口
    virtual ~BaseCallData() = default;  // 虚析构函数
};

template <typename ServiceType, typename RequestType, typename ResponseType>
class CallData : public BaseCallData {
public:
    CallData(ServiceType* service, grpc::ServerCompletionQueue* cq)
        : service(service), completeQueue(cq), responder(&context), status(CREATE) {
        Proceed();
    }

    void Proceed() {
        if (status == CREATE) {
            status = PROCESS;
            registerRpcHandler();
        }
        else if (status == PROCESS) {
            new CallData(service, completeQueue);
            proccessRequest();
            status = FINISH;
            responder.Finish(response, grpc::Status::OK, this);
        }
        else {
            if (status != FINISH) {
                //TODO:结束
            }
            delete this;
        }
    }

private:
    void registerRpcHandler() {
        if constexpr (std::is_same_v<RequestType, user::auth::getAdminTokenReq>) {
            service->RequestgetAdminToken(&context, &request, &responder, completeQueue, completeQueue, this);
        }
        else if constexpr (std::is_same_v<RequestType, user::auth::parseTokenReq>) {
            service->RequestparseToken(&context, &request, &responder, completeQueue, completeQueue, this);
        }
    }

    void proccessRequest() {
        //TODO:处理请求
    }

    ServiceType *service;
    grpc::ServerCompletionQueue *completeQueue;
    grpc::ServerContext context;
    RequestType request;
    ResponseType response;
    grpc::ServerAsyncResponseWriter<ResponseType> responder;

    enum CallStatus { CREATE, PROCESS, FINISH };
    CallStatus status;
};


#endif //CALLDATA_H
