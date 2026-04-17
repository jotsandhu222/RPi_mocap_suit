import React, { useState, useEffect } from 'react';
import { View, Text, FlatList, TouchableOpacity, StyleSheet, ActivityIndicator, Alert } from 'react-native';
import UploadButton from "../components/UploadButton";
import { Folder, FileText, Image as ImageIcon, Film, ChevronLeft, Upload, Trash2 } from 'lucide-react-native';
import * as SecureStore from 'expo-secure-store';
import axios from 'axios';

export default function FilesScreen({ navigation }) {
  const [files, setFiles] = useState([]);
  const [currentPath, setCurrentPath] = useState('');
  const [loading, setLoading] = useState(false);
  const [serverIp, setServerIp] = useState('');
  const [token, setToken] = useState('');

  useEffect(() => {
    loadCredentials();
  }, []);

  useEffect(() => {
    if (serverIp && token) {
      fetchFiles();
    }
  }, [currentPath, serverIp, token]);

  const loadCredentials = async () => {
    const ip = await SecureStore.getItemAsync('server_ip');
    const t = await SecureStore.getItemAsync('auth_token');
    if (ip) setServerIp(ip);
    if (t) setToken(t);
  };

  const fetchFiles = async () => {
    setLoading(true);
    try {
      const res = await axios.get(`http://${serverIp}:8000/api/v1/files/list`, {
        params: { path: currentPath },
        headers: { Authorization: `Bearer ${token}` }
      });
      setFiles(res.data);
    } catch (error) {
      console.error(error);
      Alert.alert('Error', 'Failed to load files');
    } finally {
      setLoading(false);
    }
  };

  const formatBytes = (bytes) => {
    if (!+bytes) return '0 Bytes';
    const k = 1024;
    const sizes = ['Bytes', 'KB', 'MB', 'GB', 'TB'];
    const i = Math.floor(Math.log(bytes) / Math.log(k));
    return `${parseFloat((bytes / Math.pow(k, i)).toFixed(2))} ${sizes[i]}`;
  };

  const getIcon = (file) => {
    if (file.is_dir) return <Folder color="#3b82f6" size={24} />;
    const ext = file.name.split('.').pop().toLowerCase();
    if (['jpg', 'jpeg', 'png', 'gif', 'webp'].includes(ext)) return <ImageIcon color="#a855f7" size={24} />;
    if (['mp4', 'webm', 'mov'].includes(ext)) return <Film color="#ef4444" size={24} />;
    return <FileText color="#6b7280" size={24} />;
  };

  const handleFilePress = (file) => {
    if (file.is_dir) {
      setCurrentPath(file.path);
    } else {
      const ext = file.name.split('.').pop().toLowerCase();
      if (['jpg', 'jpeg', 'png', 'gif', 'webp', 'mp4', 'webm', 'mov'].includes(ext)) {
        navigation.navigate('MediaViewer', { file, serverIp, token });
      } else {
        Alert.alert("Download", `Download ${file.name}?`, [
          { text: "Cancel", style: "cancel" },
          { text: "OK", onPress: () => console.log("Download stub") }
        ]);
      }
    }
  };

  const goUp = () => {
    const parts = currentPath.split('/').filter(Boolean);
    parts.pop();
    setCurrentPath(parts.join('/'));
  };

  const renderItem = ({ item }) => (
    <TouchableOpacity style={styles.fileItem} onPress={() => handleFilePress(item)}>
      <View style={styles.fileLeft}>
        {getIcon(item)}
        <View style={styles.fileInfo}>
          <Text style={styles.fileName}>{item.name}</Text>
          {!item.is_dir && <Text style={styles.fileSize}>{formatBytes(item.size)}</Text>}
        </View>
      </View>
    </TouchableOpacity>
  );

  return (
    <View style={styles.container}>
      <View style={styles.header}>
        {currentPath !== '' ? (
          <TouchableOpacity onPress={goUp} style={styles.backButton}>
            <ChevronLeft color="#000" size={24} />
          </TouchableOpacity>
        ) : <View style={{width: 24, padding: 10}}/>}
        <Text style={styles.pathText} numberOfLines={1} ellipsizeMode="head">
          / {currentPath}
        </Text>
        <UploadButton
          currentPath={currentPath}
          serverIp={serverIp}
          token={token}
          onUploadComplete={fetchFiles}
        />
      </View>

      {loading ? (
        <ActivityIndicator size="large" color="#4f46e5" style={{marginTop: 20}} />
      ) : (
        <FlatList
          data={files}
          keyExtractor={(item) => item.path}
          renderItem={renderItem}
          ListEmptyComponent={<Text style={styles.emptyText}>Folder is empty</Text>}
          contentContainerStyle={{ paddingBottom: 100 }}
        />
      )}
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: '#f9fafb',
  },
  header: {
    flexDirection: 'row',
    alignItems: 'center',
    padding: 15,
    backgroundColor: '#fff',
    borderBottomWidth: 1,
    borderBottomColor: '#e5e7eb',
  },
  backButton: {
    padding: 5,
    marginRight: 10,
  },
  pathText: {
    fontSize: 16,
    fontWeight: '500',
    color: '#374151',
    flex: 1,
  },
  fileItem: {
    flexDirection: 'row',
    alignItems: 'center',
    justifyContent: 'space-between',
    padding: 15,
    backgroundColor: '#fff',
    borderBottomWidth: 1,
    borderBottomColor: '#f3f4f6',
  },
  fileLeft: {
    flexDirection: 'row',
    alignItems: 'center',
    flex: 1,
  },
  fileInfo: {
    marginLeft: 15,
    flex: 1,
  },
  fileName: {
    fontSize: 16,
    color: '#111827',
  },
  fileSize: {
    fontSize: 12,
    color: '#6b7280',
    marginTop: 2,
  },
  emptyText: {
    textAlign: 'center',
    marginTop: 40,
    color: '#9ca3af',
  }
});
