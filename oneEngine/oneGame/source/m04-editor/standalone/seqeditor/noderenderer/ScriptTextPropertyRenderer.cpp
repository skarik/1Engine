#include "./ScriptTextPropertyRenderer.h"
#include "./NodeBoardRenderer.h"

#include "renderer/texture/RrFontTexture.h"

void m04::editor::sequence::ScriptTextPropertyRenderer::OnClicked ( const ui::eventide::Element::EventMouse& mouse_event )
{
	typedef ui::eventide::Element::EventMouse EventMouse;

	ARCORE_ASSERT(mouse_event.type == EventMouse::Type::kClicked);

	if (mouse_event.button == core::kMBLeft)
	{
		m_propertyState->m_editing = true;
	}
}

struct TextRequest
{
	std::string		text;
	Vector2f		offset;
};

struct SplitStringOutputs
{
	std::vector<TextRequest>* lines = nullptr;
};

struct TextLayoutState
{
	int				index;
	int				chara;
	Vector2f		pen;
};

template <bool bOutputLines>
void SplitString ( const char* str, RrFontTexture* font_texture, const Real max_pen_width, SplitStringOutputs& params, std::function<bool(const TextLayoutState&)>&& earlyQuitCheck = nullptr )
{
	//bool bOutputLines = true;
	bool bIterateCharacters = false;

	// Get string info
	const size_t l_strLen = strlen(str);

	// Initialize the iteration state
	TextLayoutState state;

	// Get the font info:
	auto fontInfo		= font_texture->GetFontInfo();
	auto fontStartGlyph	= font_texture->GetFontGlyphStart();
	auto fontGlyphCount	= font_texture->GetFontGlyphCount();
	Real baseScale		= (Real)font_texture->GetWidth();

	// Set up the string positions
	uint32_t string_start = 0;
	uint32_t string_length = 0;
	uint32_t line_count = 0;

	// Loop through string until we're past the end
	for (state.index = 0; state.index < l_strLen; )
	{
		// Declare helpers
		auto CharacterLookupInSet = [&](const int lookup)
		{
			return lookup >= 0 && lookup < (int)fontGlyphCount;
		};

		auto GetNextWordLength = [&](Vector2f& out_offset) -> uint32_t
		{
			for (int i = state.index; i < l_strLen; ++i)
			{
				// Update state
				state.chara = str[i];
				TextLayoutState l_state = state;
				state.index = i;
				state.pen += out_offset;
				// Do early quit check
				if (earlyQuitCheck && earlyQuitCheck(state))
				{
					state = l_state;
					uint32_t word_len = (uint32_t)(i - state.index);
					state.index = (int)(l_strLen + 1);
					return word_len;
				}
				state = l_state;

				// If it's a space, we hit end of the word.
				if (std::isspace(str[i]))
				{
					return (uint32_t)(i - state.index);
				}
				// Otherwise, we just increase the offset of the word.
				else
				{
					// Get the current character offset into the existing character set
					int i_lookup = str[i] - fontStartGlyph;

					// Check that character is in set
					if (!CharacterLookupInSet(i_lookup))
					{
						continue; // Skip quad
					}

					// Add the character's offset
					out_offset += Vector2f(fontInfo->glyphAdvance[i_lookup].x, fontInfo->glyphAdvance[i_lookup].y);
				}
			}
			return (uint32_t)(l_strLen - state.index);
		};

		auto AddNewLineAndReset = [&]()
		{
			if (bOutputLines)
			{
				// Add new line to the draw list
				params.lines->push_back(TextRequest{
					std::string(str + string_start, string_length),
					Vector2f(0.0F, (Real)(line_count * fontInfo->height))
					});
			}
			// Increment number of lines
			line_count += 1;

			// Reset pen to the beginning of the line
			state.pen.x = 0;
			state.pen.y += fontInfo->height;

			// Set new start of the word
			string_start = state.index;
			// Set new length of the word
			string_length = 0;
		};

		// Check the next character
		if (!std::isspace(str[state.index]))
		{
			// If not a space, then get the next word.
			Vector2f l_wordSize;
			uint32_t l_nextWordLength = GetNextWordLength(l_wordSize);

			// Word is not smaller than max with? Add it normally.
			if (state.pen.x + l_wordSize.x < max_pen_width)
			{
				state.pen += l_wordSize;

				state.index += l_nextWordLength;
				string_length += l_nextWordLength;
			}
			// We go beyond, and the word is small enough to wrap
			else if (state.pen.x > max_pen_width * 0.5F)
			{
				AddNewLineAndReset(); // This does not increment so the next iteration can work
			}
			// We go beyond, and the word cannot wrap
			else
			{
				// We must iterate forward until we hit the limit.
				while (true)
				{
					// Get the current character offset into the existing character set
					int c_lookup = str[state.index] - fontStartGlyph;

					// Check that character is in set
					if (!CharacterLookupInSet(c_lookup))
					{
						++state.index;
						continue; // Skip counting this character.
					}

					// Grab offset & check against limit
					Vector2f l_letterSize (fontInfo->glyphAdvance[c_lookup].x, fontInfo->glyphAdvance[c_lookup].y);
							
					// We haven't hit the limit yet, so step forward
					if (state.pen.x + l_letterSize.x < max_pen_width)
					{
						state.pen += l_letterSize;

						state.index += 1;
						string_length += 1;
					}
					// We've hit the limit, so break out of the loop.
					else
					{
						break;
					}
				}

				AddNewLineAndReset(); // This does not increment so the next iteration can work
			}
		}
		else if (str[state.index] == '\r' || str[state.index] == '\n')
		{
			// Skip to next character.
			state.index += 1;

			// Since a newline, go to the next line.
			AddNewLineAndReset();

			// Update state
			state.chara = str[state.index];
			// Do early quit check
			if (earlyQuitCheck && earlyQuitCheck(state))
			{
				state.index = (int)(l_strLen + 1);
				break;
			}
		}
		else
		{
			// Get the current character offset into the existing character set
			int c_lookup = str[state.index] - fontStartGlyph;
					
			// Grab offset & check against limit
			Vector2f l_letterSize (fontInfo->glyphAdvance[c_lookup].x, fontInfo->glyphAdvance[c_lookup].y);

			// We haven't hit the limit yet, so step forward
			if (state.pen.x + l_letterSize.x < max_pen_width)
			{
				state.pen += l_letterSize;

				state.index += 1;
				string_length += 1;
			}
			// We've hit the limit, so go to next line.
			else
			{
				AddNewLineAndReset();
			}

			// Update state
			state.chara = str[state.index];
			// Do early quit check
			if (earlyQuitCheck && earlyQuitCheck(state))
			{
				state.index = (int)(l_strLen + 1);
				break;
			}
		}
	}

	// Add the final string
	if (string_length >= 0 && string_start <= l_strLen)
	{
		if (bOutputLines)
		{
			params.lines->push_back(TextRequest{
				std::string(str + string_start, string_length),
				Vector2f(0.0F, (Real)(line_count * fontInfo->height))
				});
		}
	}

	// Do final check at the very end

	// Update state
	state.chara = str[state.index];
	// Do early quit check
	if (earlyQuitCheck && earlyQuitCheck(state))
	{
		; // Nothing
	}
}

void m04::editor::sequence::ScriptTextPropertyRenderer::BuildMesh ( void )
{
	using namespace ui::eventide;

	ParamsForText textParams;
	ParamsForCube cubeParams;
	ParamsForQuad quadParams;

	quadParams = ParamsForQuad();
	quadParams.position = m_bboxAll.GetCenterPoint();
	quadParams.size = m_bboxAll.GetExtents();
	quadParams.color = m_propertyState->m_hovered ? Color(0, 0, 0, 1) : Color(0, 0, 0, 1).Lerp(DefaultStyler.box.defaultColor, 0.5F);
	buildQuad(quadParams);

	textParams = ParamsForText();
	textParams.string = m_property->label.c_str();
	textParams.font_texture = &m_nodeRenderer->GetRenderResources().m_fontTexture;
	textParams.position = m_bboxAll.GetCenterPoint() - Vector3f(m_bboxAll.GetExtents().x, m_bboxAll.GetExtents().y, 0) + Vector3f(0, 0, 0.1F);
	textParams.rotation = m_nodeRenderer->GetBBoxAbsolute().m_M.getRotator();
	textParams.size = math::lerp(0.0F, ui::eventide::DefaultStyler.text.buttonSize, ui::eventide::DefaultStyler.text.headingSize);
	textParams.alignment = AlignHorizontal::kLeft;
	textParams.color = Color(0, 0, 0, 1).Lerp(m_propertyState->m_hovered ? DefaultStyler.text.headingColor : DefaultStyler.text.headingColor.Lerp(DefaultStyler.box.defaultColor, 0.3F), 0.4F);
	buildText(textParams);

	const char* str = GetNode()->view->GetPropertyAsString(m_property->identifier);
	if (str)
	{
		textParams = ParamsForText();
		textParams.font_texture = &m_nodeRenderer->GetRenderResources().m_fontTexture;
		textParams.size = math::lerp(0.0F, ui::eventide::DefaultStyler.text.buttonSize, ui::eventide::DefaultStyler.text.headingSize);
		textParams.position = m_bboxAll.GetCenterPoint() - Vector3f(m_bboxAll.GetExtents().x, -m_bboxAll.GetExtents().y + textParams.size, 0) + Vector3f(0, 0, 0.2F);
		textParams.rotation = m_nodeRenderer->GetBBoxAbsolute().m_M.getRotator();
		textParams.alignment = AlignHorizontal::kLeft;
		textParams.color = Color(1, 1, 1, 1);

		// Split the draw into multiple inputs
		Real l_fontScaling = 1.0F;
		std::vector<TextRequest> l_lines;
		{
			// Get the font info:
			RrFontTexture* font_texture = (RrFontTexture*)m_nodeRenderer->GetRenderResources().m_fontTexture.reference;
			auto fontInfo		= font_texture->GetFontInfo();

			// Get box width
			const core::math::BoundingBox& node_bbox = m_nodeRenderer->GetBBoxAbsolute();
			const Real l_boxWidth = node_bbox.GetExtents().x * 2.0F - m_nodeRenderer->GetPadding().x * 2.0F;

			// Set up the max width we can go to
			l_fontScaling = (textParams.size / fontInfo->height);
			const Real l_maxPenWidth = l_boxWidth / l_fontScaling;

			SplitStringOutputs outputs;
			outputs.lines = &l_lines;
			SplitString<true>(str, font_texture, l_maxPenWidth, outputs, nullptr);

			// Save number of lines
			m_lineCount = (int32_t)l_lines.size();

			// Now we want to split string, but to the given character and get the output position
			SplitString<false>(str, font_texture, l_maxPenWidth, outputs, [&](const TextLayoutState& state)->bool 
			{
				if (state.index >= m_cursorPosition)
				{
					l_lines.push_back(TextRequest{
						"|",
						state.pen + Vector2f(-4.0F, 0.0F)
						});
					return true;
				}
				return false; 
			});
		}

		// Display all the lines in-order
		Vector3f l_originalTextPosition = textParams.position;
		for (uint32_t line_index = 0; line_index < l_lines.size(); ++line_index)
		{
			// Set string and draw
			textParams.position = l_originalTextPosition;
			textParams.position.x += l_lines[line_index].offset.x * l_fontScaling;
			textParams.position.y -= l_lines[line_index].offset.y * l_fontScaling;

			textParams.string = l_lines[line_index].text.c_str();
			buildText(textParams);
		}
	}
}

void m04::editor::sequence::ScriptTextPropertyRenderer::OnGameFrameUpdate ( const ui::eventide::Element::GameFrameUpdateInput& input_frame )
{
	if (m_propertyState->m_editing)
	{
		// Parse input and modify the underlying property
		std::string l_currentValue = GetNode()->view->GetPropertyAsString(m_property->identifier);

		// Run the parse loop (see dusk/TextField.cpp)
		const auto& inputString = core::Input::FrameInputString();
		for (const auto& input : inputString)
		{
			if ( input && isprint(input) )
			{
				//l_currentValue += input;
				l_currentValue.insert(m_cursorPosition, 1, input);
				m_cursorPosition += 1;
			}
			else if ( input == core::kVkBackspace )
			{
				if (m_cursorPosition == 0)
				{
					// Nothing.
				}
				else if (m_cursorPosition < l_currentValue.length())
				{	// Delete from center
					l_currentValue =
						l_currentValue.substr(0, m_cursorPosition - 1)
						+ l_currentValue.substr(m_cursorPosition);
					m_cursorPosition -= 1;
				}
				else
				{	// Delete from end
					l_currentValue =
						l_currentValue.substr(0, m_cursorPosition - 1);
					m_cursorPosition -= 1;
				}
			}
			else if ( input == core::kVkReturn )
			{
				//l_currentValue += '\n';
				l_currentValue.insert(m_cursorPosition, 1, '\n');
				m_cursorPosition += 1;
			}
		}

		// Update other navigation input
		if (core::Input::Keydown( core::kVkLeft ))
		{
			if (m_cursorPosition > 0)
			{
				m_cursorPosition -= 1;
			}
		}
		else if (core::Input::Keydown( core::kVkRight ))
		{
			if (m_cursorPosition < l_currentValue.length())
			{
				m_cursorPosition += 1;
			}
		}

		// Save back the edits
		GetNode()->view->SetProperty(m_property->identifier, l_currentValue.c_str());
	}
}

void m04::editor::sequence::ScriptTextPropertyRenderer::UpdateLayout ( const Vector3f& upper_left_corner, const Real left_column_width, const core::math::BoundingBox& node_bbox )
{
	const Real l_boxWidth = node_bbox.GetExtents().x * 2.0F - m_nodeRenderer->GetPadding().x * 2.0F;
	int l_numberOfLines = 0;

	if (m_lineCount <= 0)
	{
		const char* l_scriptString = GetNode()->view->GetPropertyAsString(m_property->identifier);
		ParamsForText textParams;
		textParams = ParamsForText();
		textParams.string = l_scriptString;
		textParams.font_texture = &m_nodeRenderer->GetRenderResources().m_fontTexture;
		textParams.size = math::lerp(0.0F, ui::eventide::DefaultStyler.text.buttonSize, ui::eventide::DefaultStyler.text.headingSize);
		l_numberOfLines = std::max(1, (int)(predictText(textParams).x / l_boxWidth) + 1);
	}
	else
	{
		l_numberOfLines = m_lineCount;
	}

	{
		m_bboxHeight = ui::eventide::DefaultStyler.text.buttonSize * l_numberOfLines + m_nodeRenderer->GetPadding().y;
	}

	{
		m_bboxAll = core::math::BoundingBox(
			Matrix4x4(),
			upper_left_corner + Vector3f(m_nodeRenderer->GetPadding().x, 0, 0),
			upper_left_corner + Vector3f(m_nodeRenderer->GetPadding().x + l_boxWidth, -ui::eventide::DefaultStyler.text.buttonSize * l_numberOfLines, 4.0F)
		);
	}

	m_bboxKey = m_bboxAll;
}