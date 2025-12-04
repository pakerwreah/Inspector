//
// Created by Paker on 24/08/20.
//

#include "Router.h"
#include "Http.h"
#include "util.h"
#include "url.h"

Response Router::handle(const Request &request) const {
    if (request.method.empty()) {
        throw std::runtime_error("Bad Request");
    }

    Params params;
    const std::vector<std::string> &pieces = util::split(request.path, '?');
    const std::string &path = pieces[0];

    // extract query params
    if (request.method == "GET" && pieces.size() > 1) {
        params = url::params(pieces[1]);
    } else {
        // extract body params
        const auto &it = request.headers.find(Http::ContentType::Key);
        if (it != request.headers.end() && it->second.find(Http::ContentType::URL_ENCODED) != std::string::npos) {
            params = url::params(request.body);
        }
    }

    // find handler
    const std::vector<std::string> &path_pieces = util::split(path, '/');
    const size_t path_size = path_pieces.size();

    if (const auto &it = routes.find(request.method); it != routes.end()) {
        for (const auto &[path, handler] : it->second) {
            const std::vector<std::string> &route_pieces = util::split(path, '/');
            if (path_size != route_pieces.size()) continue;
            Params m_params;
            for (int i = 0; i < path_size; i++) {
                const std::string &rp = route_pieces[i];
                const std::string &pp = path_pieces[i];
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
    }

    throw std::out_of_range("Route not found");
}

void Router::route(const std::string &method, const std::string &path, Handler handler) {
    routes[method][path] = std::move(handler);
}

void Router::get(const std::string &path, Handler handler) {
    route("GET", path, std::move(handler));
}

void Router::post(const std::string &path, Handler handler) {
    route("POST", path, std::move(handler));
}

void Router::put(const std::string &path, Handler handler) {
    route("PUT", path, std::move(handler));
}
