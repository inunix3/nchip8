// Copyright (c) 2024 inunix3.
// This file is distributed under the MIT license (https://opensource.org/license/mit/)

#include <nchip8/application.hpp>

using namespace nchip8;

Application::~Application() {

}

void Application::run() {
    while (!m_quit) {
        update();
        render();
    }

    deinit();
}

void Application::update() {

}

void Application::render() {

}

void Application::deinit() {

}
