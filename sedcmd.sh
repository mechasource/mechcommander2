#!/bin/sh

find ./source -type f \( -name "*.h" -o -name "*.hpp" -o -name "*.cpp" \) -exec sed -i \
-e 's|\<gosAudio_CachedFile\>|gosAudio_ResourceType::cachedfile|g' \
-e 's|\<gosAudio_UserMemory\>|gosAudio_ResourceType::usermemory|g' \
-e 's|\<gosAudio_UserMemoryDecode\>|gosAudio_ResourceType::usermemorydecode|g' \
-e 's|\<gosAudio_UserMemoryPlayList\>|gosAudio_ResourceType::usermemoryplaylist|g' \
-e 's|\<gosAudio_StreamedFile\>|gosAudio_ResourceType::streamedfile|g' \
-e 's|\<gosAudio_StreamedMusic\>|gosAudio_ResourceType::streamedmusic|g' \
-e 's|\<gosAudio_StreamedFP\>|gosAudio_ResourceType::streamedfp|g' \
    {} \;

