#pragma once
#include "Level/Level.h"

class TestLevel : public Wanted::Level
{
friend class RTTI;
protected:
    static const size_t TypeIdClass()
    {
        static int runTimeTypeId = 0;
        return reinterpret_cast<size_t>(&runTimeTypeId);
    }
public:
    virtual const size_t& GetType() const override { return TestLevel::TypeIdClass(); }
    using super = Level;
    virtual bool Is(const size_t id) const override
    {
        if (id == TypeIdClass()) {
            return true;
        }
        else {
            return Level::Is(id);
        }
    }
    virtual bool Is(RTTI* const rtti) const override
    {
        return Is(rtti->GetType());
    }

public:
	TestLevel();
};

