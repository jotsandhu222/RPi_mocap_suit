import React, { useState } from 'react';
import { View, Image, StyleSheet, Dimensions, ActivityIndicator } from 'react-native';
import { Video } from 'expo-av';

const { width, height } = Dimensions.get('window');

export default function MediaViewerScreen({ route, navigation }) {
  const { file, serverIp, token } = route.params;
  const [loading, setLoading] = useState(true);

  // Determine media type
  const isVideo = file.name.match(/\.(mp4|webm|mov)$/i);
  const isImage = file.name.match(/\.(jpg|jpeg|png|gif|webp)$/i);

  const streamUrl = `http://${serverIp}:8000/api/v1/files/stream?path=${encodeURIComponent(file.path)}&token=${token}`;

  return (
    <View style={styles.container}>
      {isImage && (
        <Image
          source={{ uri: streamUrl }}
          style={styles.image}
          resizeMode="contain"
          onLoadEnd={() => setLoading(false)}
        />
      )}

      {isVideo && (
        <Video
          source={{ uri: streamUrl }}
          style={styles.video}
          useNativeControls
          resizeMode="contain"
          isLooping={false}
          onLoad={() => setLoading(false)}
        />
      )}

      {loading && (
        <ActivityIndicator size="large" color="#fff" style={styles.loader} />
      )}
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: '#000',
    justifyContent: 'center',
    alignItems: 'center'
  },
  image: {
    width: width,
    height: height,
  },
  video: {
    width: width,
    height: height * 0.8,
  },
  loader: {
    position: 'absolute'
  }
});
