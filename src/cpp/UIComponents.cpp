#include "UIComponents.h"

#include "Constants.h"

const std::vector<std::string>& UIComponents::getCategories() {
    return Constants::getCategoryNames();
}
