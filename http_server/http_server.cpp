// http_server.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <httplib.h>
#include <Windows.h>

#include "Point.h"

std::string dump_headers(const httplib::Headers &headers) {
	std::string s;
	char buf[BUFSIZ];

	for (auto it = headers.begin(); it != headers.end(); ++it) {
		const auto &x = *it;
		snprintf(buf, sizeof(buf), "%s: %s\n", x.first.c_str(), x.second.c_str());
		s += buf;
	}

	return s;
}

std::string log(const httplib::Request &req, const httplib::Response &res) {
	std::string s;
	char buf[BUFSIZ];

	s += "================================\n";

	snprintf(buf, sizeof(buf), "%s %s %s", req.method.c_str(),
		req.version.c_str(), req.path.c_str());
	s += buf;

	std::string query;
	for (auto it = req.params.begin(); it != req.params.end(); ++it) {
		const auto &x = *it;
		snprintf(buf, sizeof(buf), "%c%s=%s",
			(it == req.params.begin()) ? '?' : '&', x.first.c_str(),
			x.second.c_str());
		query += buf;
	}
	snprintf(buf, sizeof(buf), "%s\n", query.c_str());
	s += buf;

	//s += dump_headers(req.headers);

	s += "--------------------------------\n";

	snprintf(buf, sizeof(buf), "%d %s\n", res.status, res.version.c_str());
	s += buf;
	s += dump_headers(res.headers);
	s += "\n";

	if (!res.body.empty()) { s += res.body; }

	s += "\n";

	return s;
}

int snapshot_ok(void) {
	Point p1(0, 0);
	p1.move(3, 5);
	printf("get_x:%d, get_y:%d\n", p1.get_x(), p1.get_y());
	return 0;
}

int snapshot_err(void) {
	throw std::runtime_error("failed!");
	return -1;
}

int main(void) {
	httplib::Server svr;

	if (!svr.is_valid()) {
		printf("server has an error...\n");
		return -1;
	}

	svr.Get("/snapshot_ok", [](const httplib::Request & /*req*/, httplib::Response &res) {
		snapshot_ok();
		res.set_content("{\"result\":\"OK\"}", "application/json");
	});

	svr.Get("/snapshot_err", [](const httplib::Request & /*req*/, httplib::Response &res) {
		snapshot_err();
		res.set_content("{\"result\":\"OK\"}", "application/json");
	});

	svr.set_error_handler([](const httplib::Request & /*req*/, httplib::Response &res) {
		const char *fmt = "{\"result\":\"ERROR\", \"status\":%d}";
		char buf[BUFSIZ];
		snprintf(buf, sizeof(buf), fmt, res.status);
		res.set_content(buf, "application/json");
	});

	svr.set_logger([](const httplib::Request &req, const httplib::Response &res) {
		printf("%s", log(req, res).c_str());
	});

	svr.Get("/stop", [&](const httplib::Request& req, httplib::Response& res) {
		res.set_content("{\"result\":\"OK\"}", "application/json");
		svr.stop();
	});

	svr.listen("localhost", 8080);
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started:
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file