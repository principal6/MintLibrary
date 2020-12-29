namespace fs
{
	namespace SimpleRendering
	{
		FS_INLINE void IRenderer::setPosition(const fs::Float3& position) noexcept
		{
			_position = position;
		}

		FS_INLINE const fs::Float4& IRenderer::getColorInternal(const uint32 index) const noexcept
		{
			const uint32 colorCount = static_cast<uint32>(_colorArray.size());
			return (colorCount <= index) ? _defaultColor : _colorArray[index];
		}
	}
}
