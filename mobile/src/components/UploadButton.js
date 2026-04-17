import React, { useState } from 'react';
import { TouchableOpacity, ActivityIndicator, Alert, View, Text } from 'react-native';
import { Upload } from 'lucide-react-native';
import * as ImagePicker from 'expo-image-picker';
import axios from 'axios';

export default function UploadButton({ currentPath, serverIp, token, onUploadComplete }) {
  const [uploading, setUploading] = useState(false);

  const pickImage = async () => {
    let result = await ImagePicker.launchImageLibraryAsync({
      mediaTypes: ImagePicker.MediaTypeOptions.All,
      allowsEditing: false,
      quality: 1,
    });

    if (!result.canceled) {
      handleUpload(result.assets[0]);
    }
  };

  const handleUpload = async (asset) => {
    setUploading(true);

    // Create form data
    const formData = new FormData();
    const filename = asset.fileName || asset.uri.split('/').pop();

    formData.append('file', {
      uri: asset.uri,
      name: filename,
      type: asset.mimeType || 'application/octet-stream'
    });
    formData.append('path', currentPath);

    try {
      await axios.post(`http://${serverIp}:8000/api/v1/files/upload`, formData, {
        headers: {
          'Content-Type': 'multipart/form-data',
          'Authorization': `Bearer ${token}`
        }
      });
      onUploadComplete();
    } catch (error) {
      console.error(error);
      Alert.alert('Upload Failed', 'There was an error uploading the file.');
    } finally {
      setUploading(false);
    }
  };

  return (
    <TouchableOpacity
      onPress={pickImage}
      disabled={uploading}
      style={{
        flexDirection: 'row',
        alignItems: 'center',
        backgroundColor: '#4f46e5',
        paddingHorizontal: 15,
        paddingVertical: 8,
        borderRadius: 8,
        marginRight: 10
      }}
    >
      {uploading ? (
        <ActivityIndicator size="small" color="#fff" />
      ) : (
        <>
          <Upload color="#fff" size={18} />
          <Text style={{color: '#fff', marginLeft: 5, fontWeight: '500'}}>Upload</Text>
        </>
      )}
    </TouchableOpacity>
  );
}
