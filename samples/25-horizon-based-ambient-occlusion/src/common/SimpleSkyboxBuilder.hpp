#pragma once

#include "stdafx.hpp"

#include "AbstractSkyboxBuilder.hpp"

class SimpleSkyboxBuilder : public AbstractSkyboxBuilder
{
public:
    SimpleSkyboxBuilder();

    SimpleSkyboxBuilder* top(std::string filename);

    SimpleSkyboxBuilder* bottom(std::string filename);

    SimpleSkyboxBuilder* left(std::string filename);

    SimpleSkyboxBuilder* right(std::string filename);

    SimpleSkyboxBuilder* front(std::string filename);

    SimpleSkyboxBuilder* back(std::string filename);

protected:
    globjects::Texture* getTexture() override;

private:
    sf::Image m_top, m_bottom, m_left, m_right, m_front, m_back;
};
