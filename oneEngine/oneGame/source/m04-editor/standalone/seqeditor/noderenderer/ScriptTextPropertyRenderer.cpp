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
			for ( int c = 0; c < l_strLen; ++c, ++string_length )
			{
				// Get the current character offset into the existing character set
				int c_lookup = str[c] - fontStartGlyph;

				// If not a space, then lookahead for the next space
				if (!std::isspace(str[c]))
				{
					// Check that character is in set
					if ( c_lookup < 0 || c_lookup >= (int)fontGlyphCount )
					{
						continue; // Skip quad
					}

					Vector2f l_nextPen = pen;

					for (int next_c = c; next_c < l_strLen; ++next_c)
					{
						// Get the current character offset into the existing character set
						int next_c_lookup = str[next_c] - fontStartGlyph;
						
						if (std::isspace(str[next_c]))
						{
							if (str[next_c] == '\r' || str[next_c] == '\n')
							{
								// If it's a newline, we need to push back a new word and break out now.
								int word_length = next_c - c;
								c += word_length;
								string_length += word_length;

								l_lines.push_back(std::string(str + string_start, string_length));
								string_length = 0;
								string_start = c;

								pen.x = 0;
								pen.y += fontInfo->height + 3.0F;
								break;
							}
							else
							{
								// It's a space, we can break out of this check loop and add in the word.
								int word_length = next_c - c;
								c += word_length;
								string_length += word_length;
							}
							break;
						}
						else
						{
							// Check that character is in set
							if ( next_c_lookup < 0 || next_c_lookup >= (int)fontGlyphCount )
							{
								continue; // Skip quad
							}

							l_nextPen += Vector2f(fontInfo->glyphAdvance[next_c_lookup].x, fontInfo->glyphAdvance[next_c_lookup].y);

							// If the pen is going beyond the limit, then this word needs to move to the next line.
							if (l_nextPen.x > l_maxPenWidth)
							{
								// Word can go onto the next line.
								if (pen.x > l_maxPenWidth / 2.0F)
								{
									l_lines.push_back(std::string(str + string_start, string_length));
									string_length = 0;
									string_start = c;

									pen.x = (Real)fontInfo->glyphAdvance[c_lookup].x;
									pen.y += fontInfo->height + 3.0F;
									break;
								}
								// Word is too long to go onto the next line.
								else
								{
									int word_length = next_c - c;
									c += word_length;
									string_length += word_length;

									l_lines.push_back(std::string(str + string_start, string_length));
									string_length = 0;
									string_start = c;

									pen.x = (Real)fontInfo->glyphAdvance[c_lookup].x;
									pen.y += fontInfo->height + 3.0F;
									break;
								}
							}
						}
					}
				}
				else
				{
					if (str[c] == '\r' || str[c] == '\n')
					{
						pen.x = 0.0F;
						pen.y += fontInfo->height + 3.0F;
						l_lines.push_back("");
					}
					else
					{
						pen += Vector2f(fontInfo->glyphAdvance[c_lookup].x, fontInfo->glyphAdvance[c_lookup].y);
					}
				}

				// Get the next position
				/*Vector2f l_nextPen = pen + Vector2f(fontInfo->glyphAdvance[c_lookup].x, fontInfo->glyphAdvance[c_lookup].y);

				// If the pen is going beyond the limit, split the string.
				if (l_nextPen.x > l_maxPenWidth)
				{
					l_lines.push_back(std::string(str + string_start, string_length));
					string_length = 0;
					string_start = c;

					pen.x = fontInfo->glyphAdvance[c_lookup].x;
					pen.y += fontInfo->height + 3.0F;
				}
				else
				{
					pen = l_nextPen;
				}*/
			}

			// Add the final string
			if (string_length > 0 && string_start < l_strLen)
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