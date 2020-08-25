//
// Created by Paker on 24/08/20.
//

#include "Router.h"
#include "Http.h"
#include "util.h"
#include "url.h"

using namespace std;

Params RouteParser::decode(const string &urlencoded) {
    Params params;
    auto pieces = util::split(urlencoded, '&');
    for (const string &piece : pieces) {
        auto p = util::split(piece, '=');
        params[p[0]] = p.size() == 2 ? url::decode(p[1]) : "";
    }
    return params;
}

pair<Route, Params> RouteParser::parse(const vector<Route> &routes, const Request &request) {
    Params params;
    auto pieces = util::split(request.path, '?');
    auto path = pieces[0];

    // extract query params
    if (request.method == "GET" && pieces.size() > 1) {
        util::merge(params, decode(pieces[1]));
    } else {
        // extract body params
        auto type = request.headers.find("Content-Type");
        if (type != request.headers.end() && type->second.find(Http::ContentType::URL_ENCODED) != string::npos) {
            util::merge(params, decode(request.body));
        }
    }

    // find handler
    auto path_pieces = util::split(path, '/');

    for (const auto &route : routes) {
        if (route.method != "*" && route.method != request.method) continue;

        auto route_pieces = util::split(route.path, '/');
        auto path_size = path_pieces.size();

        if (route_pieces.back() == "*") { // subroute
            if (path_size < route_pieces.size()) continue;
            route_pieces.pop_back();
            path_size = route_pieces.size();
        } else if (path_size != route_pieces.size()) continue;

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
                return {route, params};
            }
        }
    }

    throw out_of_range("Route not found");
}
