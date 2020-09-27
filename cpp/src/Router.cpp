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
        throw runtime_error("Bad Request");
    }

    Params params;
    const vector<string> &pieces = util::split(request.path, '?');
    const string &path = pieces[0];

    // extract query params
    if (request.method == "GET" && pieces.size() > 1) {
        params = url::params(pieces[1]);
    } else {
        // extract body params
        const auto &it = request.headers.find(Http::ContentType::Key);
        if (it != request.headers.end() && it->second.find(Http::ContentType::URL_ENCODED) != string::npos) {
            params = url::params(request.body);
        }
    }

    // find handler
    const vector<string> &path_pieces = util::split(path, '/');
    const size_t path_size = path_pieces.size();
    const auto &it = routes.find(request.method);

    if (it != routes.end())
        for (const auto &[path, handler] : it->second) {
            const vector<string> &route_pieces = util::split(path, '/');
            if (path_size != route_pieces.size()) continue;
            Params m_params;
            for (int i = 0; i < path_size; i++) {
                const string &rp = route_pieces[i];
                const string &pp = path_pieces[i];
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
