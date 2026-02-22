// This file is part of the course TPV2@UCM - Samir Genaim

#pragma once

class FighterFacade {
public:
    FighterFacade() {}
    virtual ~FighterFacade() {}

    virtual void create_fighter() = 0;
    virtual void reset_fighter() = 0;
    virtual void reset_lives() = 0;
    virtual int  update_lives(int n) = 0;
};