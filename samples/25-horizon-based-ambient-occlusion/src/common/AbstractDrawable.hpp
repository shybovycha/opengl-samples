#pragma once

class AbstractDrawable
{
public:
    virtual void draw() = 0;

    virtual void drawInstanced(unsigned int instances) = 0;

    virtual void bind() = 0;

    virtual void unbind() = 0;
};
