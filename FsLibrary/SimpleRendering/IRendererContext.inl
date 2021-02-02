namespace fs
{
	namespace SimpleRendering
	{
		FS_INLINE void IRendererContext::setUseMultipleViewports() noexcept
		{
			_useMultipleViewports = true;
		}

		FS_INLINE const bool IRendererContext::getUseMultipleViewports() const noexcept
		{
			return _useMultipleViewports;
		}

		FS_INLINE void IRendererContext::setPosition(const fs::Float3& position) noexcept
		{
			_position = position;
		}

		FS_INLINE void IRendererContext::setPositionZ(const float s) noexcept
		{
			_position._z = s;
		}

		FS_INLINE const fs::Float4& IRendererContext::getColorInternal(const uint32 index) const noexcept
		{
			const uint32 colorCount = static_cast<uint32>(_colorArray.size());
			return (colorCount <= index) ? _defaultColor : _colorArray[index];
		}
	}
}
