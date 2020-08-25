//
// Created by Paker on 25/08/20.
//

#ifndef INSPECTOR_HTTPROUTER_H
#define INSPECTOR_HTTPROUTER_H

#include "Router.h"
#include "Request.h"
#include "Response.h"
#include <functional>

typedef Router<std::function<Response(const Request &, const Params &)>> HttpRouter;

#endif //INSPECTOR_HTTPROUTER_H
