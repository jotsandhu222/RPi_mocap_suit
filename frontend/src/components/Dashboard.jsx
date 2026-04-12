import { useState, useEffect, useRef } from 'react';
import axios from 'axios';
import { Folder, FileText, Upload, Plus, Trash2, ArrowLeft, Image as ImageIcon, Film } from 'lucide-react';

const api = axios.create({
  baseURL: '/api/v1'
});

api.interceptors.request.use(config => {
  config.headers.Authorization = `Bearer ${localStorage.getItem('token')}`;
  return config;
});

export default function Dashboard() {
  const [files, setFiles] = useState([]);
  const [currentPath, setCurrentPath] = useState('');
  const [status, setStatus] = useState(null);
  const fileInputRef = useRef();

  useEffect(() => {
    fetchFiles();
    fetchStatus();
  }, [currentPath]);

  const fetchFiles = async () => {
    try {
      const res = await api.get(`/files/list?path=${encodeURIComponent(currentPath)}`);
      setFiles(res.data);
    } catch (err) {
      console.error(err);
    }
  };

  const fetchStatus = async () => {
    try {
      const res = await api.get('/system/status');
      setStatus(res.data);
    } catch (err) {
      console.error(err);
    }
  };

  const handleUpload = async (e) => {
    const file = e.target.files[0];
    if (!file) return;

    const formData = new FormData();
    formData.append('file', file);
    formData.append('path', currentPath);

    try {
      await api.post('/files/upload', formData, {
        headers: { 'Content-Type': 'multipart/form-data' }
      });
      fetchFiles();
    } catch (err) {
      console.error('Upload failed', err);
    }
  };

  const handleCreateFolder = async () => {
    const name = prompt('Folder name:');
    if (!name) return;
    try {
      await api.post(`/files/mkdir?path=${encodeURIComponent(currentPath)}&folder_name=${encodeURIComponent(name)}`);
      fetchFiles();
    } catch (err) {
      console.error('Create folder failed', err);
    }
  };

  const handleDelete = async (filePath) => {
    if (!confirm('Are you sure you want to delete this?')) return;
    try {
      await api.delete(`/files/delete?path=${encodeURIComponent(filePath)}`);
      fetchFiles();
    } catch (err) {
      console.error('Delete failed', err);
    }
  };

  const goUp = () => {
    const parts = currentPath.split('/').filter(Boolean);
    parts.pop();
    setCurrentPath(parts.join('/'));
  };

  const formatBytes = (bytes, decimals = 2) => {
    if (!+bytes) return '0 Bytes';
    const k = 1024, dm = decimals < 0 ? 0 : decimals, sizes = ['Bytes', 'KB', 'MB', 'GB', 'TB'];
    const i = Math.floor(Math.log(bytes) / Math.log(k));
    return `${parseFloat((bytes / Math.pow(k, i)).toFixed(dm))} ${sizes[i]}`;
  };

  const renderIcon = (file) => {
    if (file.is_dir) return <Folder className="text-blue-500" size={24} />;
    const ext = file.name.split('.').pop().toLowerCase();
    if (['jpg', 'jpeg', 'png', 'gif', 'webp'].includes(ext)) return <ImageIcon className="text-purple-500" size={24} />;
    if (['mp4', 'webm', 'mov'].includes(ext)) return <Film className="text-red-500" size={24} />;
    return <FileText className="text-gray-500" size={24} />;
  };

  return (
    <div className="flex flex-col h-full gap-6">
      {status && (
        <div className="bg-white p-4 rounded-lg shadow-sm border border-gray-100 flex justify-between items-center">
           <div>
              <p className="text-sm text-gray-500 font-medium">Storage Usage</p>
              <div className="w-64 h-2 bg-gray-200 rounded-full mt-2 overflow-hidden">
                 <div
                   className="h-full bg-indigo-500"
                   style={{ width: `${(status.used_space / status.total_space) * 100}%` }}
                 ></div>
              </div>
           </div>
           <div className="text-right">
             <p className="text-sm font-medium text-gray-900">{formatBytes(status.used_space)} used</p>
             <p className="text-xs text-gray-500">{formatBytes(status.total_space)} total</p>
           </div>
        </div>
      )}

      <div className="bg-white rounded-lg shadow-sm border border-gray-100 flex-1 overflow-hidden flex flex-col">
        <div className="p-4 border-b border-gray-100 flex justify-between items-center bg-gray-50">
          <div className="flex items-center gap-2">
            {currentPath && (
              <button onClick={goUp} className="p-1 hover:bg-gray-200 rounded">
                <ArrowLeft size={20} />
              </button>
            )}
            <span className="font-mono text-sm text-gray-700 font-medium">
              / {currentPath}
            </span>
          </div>
          <div className="flex gap-2">
            <input type="file" className="hidden" ref={fileInputRef} onChange={handleUpload} />
            <button onClick={() => fileInputRef.current.click()} className="flex items-center gap-1 text-sm bg-indigo-50 text-indigo-700 px-3 py-1.5 rounded-md hover:bg-indigo-100 font-medium">
              <Upload size={16} /> Upload
            </button>
            <button onClick={handleCreateFolder} className="flex items-center gap-1 text-sm bg-gray-100 text-gray-700 px-3 py-1.5 rounded-md hover:bg-gray-200 font-medium">
              <Plus size={16} /> New Folder
            </button>
          </div>
        </div>

        <div className="flex-1 overflow-auto">
          {files.length === 0 ? (
            <div className="p-8 text-center text-gray-500">This folder is empty.</div>
          ) : (
            <ul className="divide-y divide-gray-100">
              {files.map((file) => (
                <li key={file.path} className="p-4 flex items-center justify-between hover:bg-gray-50 group">
                  <div
                    className="flex items-center gap-3 cursor-pointer flex-1"
                    onClick={() => {
                      if (file.is_dir) {
                        setCurrentPath(file.path);
                      } else {
                        // For files, download or view
                        window.open(`/api/v1/files/stream?path=${encodeURIComponent(file.path)}&token=${localStorage.getItem('token')}`, '_blank');
                      }
                    }}
                  >
                    {renderIcon(file)}
                    <div>
                      <p className="text-sm font-medium text-gray-900">{file.name}</p>
                      <p className="text-xs text-gray-500">{!file.is_dir && formatBytes(file.size)}</p>
                    </div>
                  </div>
                  <button onClick={() => handleDelete(file.path)} className="text-gray-400 hover:text-red-500 opacity-0 group-hover:opacity-100 transition-opacity p-2">
                    <Trash2 size={18} />
                  </button>
                </li>
              ))}
            </ul>
          )}
        </div>
      </div>
    </div>
  );
}
