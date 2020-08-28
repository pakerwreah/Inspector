//
// Created by Paker on 24/08/20.
//

#include "Router.h"
#include "Http.h"
#include "util.h"
#include "url.h"

using namespace std;

Response Router::handle(const Request &request) const {
    if (request.method.empty()) {
        return Response::BadRequest();
    }

    Params params;
    auto pieces = util::split(request.path, '?');
    auto path = pieces[0];

    // extract query params
    if (request.method == "GET" && pieces.size() > 1) {
        util::merge(params, url::params(pieces[1]));
    } else {
        // extract body params
        auto type = request.headers.find(Http::ContentType::Key);
        if (type != request.headers.end() && type->second.find(Http::ContentType::URL_ENCODED) != string::npos) {
            util::merge(params, url::params(request.body));
        }
    }

    // find handler
    auto path_pieces = util::split(path, '/');
    auto path_size = path_pieces.size();

    for (const auto &[path, handler] : routes.at(request.method)) {
        auto route_pieces = util::split(path, '/');
        if (path_size != route_pieces.size()) continue;
        Params m_params;
        for (int i = 0; i < path_size; i++) {
            auto rp = route_pieces[i];
            auto pp = path_pieces[i];
            if (rp.find('{') == 0) {
                m_params[util::trim(rp, "{}")] = pp;
            } else if (rp != pp) {
                break;
            }
            if (i == path_size - 1) {
                util::merge(params, m_params);
                return handler(request, params);
            }
        }
    }

    throw out_of_range("Route not found");
}

void Router::route(const string &method, const string &path, Handler handler) {
    routes[method][path] = handler;
}

void Router::get(const string &path, Handler handler) {
    route("GET", path, handler);
}

void Router::post(const string &path, Handler handler) {
    route("POST", path, handler);
}

void Router::put(const string &path, Handler handler) {
    route("PUT", path, handler);
}
