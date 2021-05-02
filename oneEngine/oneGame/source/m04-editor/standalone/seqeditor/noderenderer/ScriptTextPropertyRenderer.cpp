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
	//textParams.position = bbox_all.GetCenterPoint() - Vector3f(bbox_all.GetExtents().x, bbox_all.GetExtents().y, -bbox_all.GetExtents().z);
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
		//textParams.string = l_drawString.c_str();
		textParams.font_texture = &m_nodeRenderer->GetRenderResources().m_fontTexture;
		//textParams.position = bbox_all.GetCenterPoint() - Vector3f(bbox_all.GetExtents().x, bbox_all.GetExtents().y, -bbox_all.GetExtents().z) + Vector3f(0, 0, 0.1F);
		textParams.size = math::lerp(0.0F, ui::eventide::DefaultStyler.text.buttonSize, ui::eventide::DefaultStyler.text.headingSize);
		textParams.position = m_bboxAll.GetCenterPoint() - Vector3f(m_bboxAll.GetExtents().x, -m_bboxAll.GetExtents().y + textParams.size, 0) + Vector3f(0, 0, 0.2F);
		textParams.rotation = m_nodeRenderer->GetBBoxAbsolute().m_M.getRotator();
		textParams.alignment = AlignHorizontal::kLeft;
		textParams.color = Color(1, 1, 1, 1);

		/*std::string l_drawString = str;
		if (m_propertyState->m_editing)
		{
			if (fmod(Time::currentTime, 1.0F) < 0.5F)
			{
				l_drawString += '|';
			}
		}*/
		const size_t l_strLen = strlen(str);

		// TODO: split the draw based on new-lines?
		std::vector<std::string> l_lines;
		{
			const core::math::BoundingBox& node_bbox = m_nodeRenderer->GetBBoxAbsolute();
			const Real l_boxWidth = node_bbox.GetExtents().x * 2.0F - m_nodeRenderer->GetPadding().x * 2.0F;

			// Get the font info:
			RrFontTexture* font_texture = (RrFontTexture*)m_nodeRenderer->GetRenderResources().m_fontTexture.reference;
			auto fontInfo		= font_texture->GetFontInfo();
			auto fontStartGlyph	= font_texture->GetFontGlyphStart();
			auto fontGlyphCount	= font_texture->GetFontGlyphCount();
			Real baseScale		= (Real)font_texture->GetWidth();

			// Set up information for the text passes:
			Vector2f pen (0,0);
			// Always use 'M' as the base case font size, because it's huge
			Vector2i font_max_size = fontInfo->glyphSize['M' - fontStartGlyph];	

			// Set up the max width we can go to
			const Real l_maxPenWidth = l_boxWidth / (textParams.size / fontInfo->height);

			// Set up the string positions
			uint32_t string_start = 0;
			uint32_t string_length = 0;

			// Loop through string until we're past the end
			for (int c = 0; c < l_strLen; )
			{
				// Declare helpers
				auto CharacterLookupInSet = [&](const int lookup)
				{
					return lookup >= 0 && lookup < (int)fontGlyphCount;
				};
				auto GetNextWordLength = [&](Vector2f& out_offset) -> uint32_t
				{
					for (int i = c; i < l_strLen; ++i)
					{
						if (std::isspace(str[i]))
						{
							return (uint32_t)(i - c);
						}
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
					return (uint32_t)(l_strLen - c);
				};
				auto AddNewLineAndReset = [&]()
				{
					// Add new line to the draw list
					l_lines.push_back(std::string(str + string_start, string_length));

					// Reset pen to the beginning of the line
					pen.x = 0;
					pen.y += fontInfo->height + 3.0F;

					// Set new start of the word
					string_start = c;
					// Set new length of the word
					string_length = 0;
				};

				// Check the next character
				if (!std::isspace(str[c]))
				{
					// If not a space, then get the next word.
					Vector2f l_wordSize;
					uint32_t l_nextWordLength = GetNextWordLength(l_wordSize);

					// Word is not smaller than max with? Add it normally.
					if (pen.x + l_wordSize.x < l_maxPenWidth)
					{
						pen += l_wordSize;

						c += l_nextWordLength;
						string_length += l_nextWordLength;
					}
					// We go beyond, and the word is small enough to wrap
					else if (pen.x > l_maxPenWidth * 0.5F)
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
							int c_lookup = str[c] - fontStartGlyph;

							// Check that character is in set
							if (!CharacterLookupInSet(c_lookup))
							{
								++c;
								continue; // Skip counting this character.
							}

							// Grab offset & check against limit
							Vector2f l_letterSize (fontInfo->glyphAdvance[c_lookup].x, fontInfo->glyphAdvance[c_lookup].y);
							
							// We haven't hit the limit yet, so step forward
							if (pen.x + l_letterSize.x < l_maxPenWidth)
							{
								pen += l_letterSize;

								c += 1;
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
				else if (str[c] == '\r' || str[c] == '\n')
				{
					// Skip to next character.
					c += 1;

					// Since a newline, go to the next line.
					AddNewLineAndReset();
				}
				else
				{
					// Get the current character offset into the existing character set
					int c_lookup = str[c] - fontStartGlyph;
					
					// Grab offset & check against limit
					Vector2f l_letterSize (fontInfo->glyphAdvance[c_lookup].x, fontInfo->glyphAdvance[c_lookup].y);

					// We haven't hit the limit yet, so step forward
					if (pen.x + l_letterSize.x < l_maxPenWidth)
					{
						pen += l_letterSize;

						c += 1;
						string_length += 1;
					}
					// We've hit the limit, so go to next line.
					else
					{
						AddNewLineAndReset();
					}
				}
			}

			// Add the final string
			if (string_length >= 0 && string_start <= l_strLen)
			{
				l_lines.push_back(std::string(str + string_start, string_length));
			}
		}

		// Save number of lines
		m_lineCount = (int32_t)l_lines.size();

		// Display all the lines in-order
		for (uint32_t line_index = 0; line_index < l_lines.size(); ++line_index)
		{
			// Set string and draw
			textParams.string = l_lines[line_index].c_str();
			buildText(textParams);
			
			// Move down to next line
			textParams.position.y -= textParams.size;
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
				l_currentValue += input;
			}
			else if ( input == core::kVkBackspace )
			{
				l_currentValue = l_currentValue.substr(0, l_currentValue.length() - 1);
			}
			else if ( input == core::kVkReturn )
			{
				l_currentValue += '\n';
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