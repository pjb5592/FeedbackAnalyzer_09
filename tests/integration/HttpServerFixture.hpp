#pragma once

#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <string>
#include <thread>
#include <vector>

#include "httplib.h"

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif

class HttpServerFixture {
public:
    explicit HttpServerFixture(const std::string& host = "127.0.0.1", int port = 8080)
        : host_(host), port_(port) {
        start();
    }

    ~HttpServerFixture() { stop(); }

    httplib::Client client() const {
        httplib::Client cli(host_, port_);
        cli.set_connection_timeout(2, 0);
        cli.set_read_timeout(5, 0);
        return cli;
    }

    bool ready() const { return ready_; }

private:
    std::string host_;
    int port_;
    bool ready_ = false;
#ifdef _WIN32
    PROCESS_INFORMATION process_{};
    bool started_ = false;
#endif

    static std::filesystem::path serverExecutable() {
#ifdef FA_SERVER_EXE
        return std::filesystem::path(FA_SERVER_EXE);
#else
        return std::filesystem::path("feedback_analyzer.exe");
#endif
    }

    void start() {
#ifdef _WIN32
        const auto exe = serverExecutable();
        if (!std::filesystem::exists(exe)) {
            return;
        }

        STARTUPINFOA si{};
        si.cb = sizeof(si);
        std::string cmd = "\"" + exe.string() + "\"";
        std::vector<char> cmdLine(cmd.begin(), cmd.end());
        cmdLine.push_back('\0');

        if (!CreateProcessA(nullptr, cmdLine.data(), nullptr, nullptr, FALSE,
                            CREATE_NEW_PROCESS_GROUP, nullptr,
                            exe.parent_path().string().c_str(), &si, &process_)) {
            return;
        }
        started_ = true;

        for (int attempt = 0; attempt < 60; ++attempt) {
            httplib::Client probe(host_, port_);
            probe.set_connection_timeout(1, 0);
            if (auto res = probe.Get("/")) {
                if (res->status == 200) {
                    ready_ = true;
                    break;
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
#endif
    }

    void stop() {
#ifdef _WIN32
        if (started_) {
            TerminateProcess(process_.hProcess, 0);
            CloseHandle(process_.hProcess);
            CloseHandle(process_.hThread);
            started_ = false;
        }
#endif
    }
};
