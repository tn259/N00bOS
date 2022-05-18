#pragma once

namespace fs {

class filesystem;

namespace fat {

filesystem* fat16_init();

}  // namespace fat
}  // namespace fs