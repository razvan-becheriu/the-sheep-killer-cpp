#ifndef UIBASE_HPP
#define UIBASE_HPP

#include <string>
#include <vector>
#include <memory>

class TUiWidget {
public:
    int x, y, width, height;
    bool visible;
    std::string hint;

    TUiWidget() : x(0), y(0), width(0), height(0), visible(true) {}
    virtual ~TUiWidget() = default;
    
    virtual void Draw() = 0;
    virtual bool ProcessEvent() { return false; }
};

class TUiWindow : public TUiWidget {
public:
    std::string title;
    virtual void Draw() override;
};

class TUiDesktop : public TUiWidget {
private:
    std::vector<TUiWidget*> widgets;
    TUiWidget* fOver;
    std::string fcurrenthint;

public:
    TUiDesktop();
    virtual ~TUiDesktop() override = default;

    virtual int Add(TUiWidget* w);
    int AddModal(TUiWindow* w);
    virtual int Remove(TUiWidget* w);

    bool IsOverWidget() const;
    
    // Getters for Pascal properties
    TUiWidget* getWidgetOver() const { return fOver; }
    std::string getCurrentHint() const { return fcurrenthint; }

    virtual void Draw() override;
};

#endif // UIBASE_HPP