#pragma once

// Umbrella header for docx-lib. Include this to get the full public API.

// Core
#include <docx/document.hpp>
#include <docx/exceptions.hpp>

// Common utilities
#include <docx/common/color.hpp>
#include <docx/common/enums.hpp>

// Document model
#include <docx/model/cell.hpp>
#include <docx/model/node.hpp>
#include <docx/model/paragraph.hpp>
#include <docx/model/run.hpp>
#include <docx/model/run_context.hpp>
#include <docx/model/table.hpp>
#include <docx/model/text.hpp>

// Open Packaging Conventions (OPC)
#include <docx/opc/package.hpp>
#include <docx/opc/part.hpp>

// Styling
#include <docx/style/border.hpp>
#include <docx/style/paragraph_properties.hpp>
#include <docx/style/run_properties.hpp>
#include <docx/style/run_types.hpp>
