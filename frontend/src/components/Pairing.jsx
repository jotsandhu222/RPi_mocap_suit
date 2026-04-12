import { useState, useEffect } from 'react';
import { QRCodeSVG } from 'qrcode.react';
import axios from 'axios';
import { Smartphone } from 'lucide-react';

export default function Pairing() {
  const [ip, setIp] = useState('');

  useEffect(() => {
    axios.get('/api/v1/system/status', {
      headers: { Authorization: `Bearer ${localStorage.getItem('token')}` }
    }).then(res => setIp(res.data.server_ip));
  }, []);

  if (!ip) return <div className="p-8 text-center text-gray-500">Loading pairing info...</div>;

  const pairingData = JSON.stringify({
    server_ip: ip,
    port: 8000
  });

  return (
    <div className="max-w-2xl mx-auto bg-white rounded-xl shadow-md overflow-hidden mt-8">
      <div className="p-8 text-center">
        <div className="mx-auto flex items-center justify-center h-12 w-12 rounded-full bg-indigo-100 mb-4">
          <Smartphone className="h-6 w-6 text-indigo-600" />
        </div>
        <h2 className="text-2xl font-bold text-gray-900 mb-2">Pair Mobile Device</h2>
        <p className="text-gray-500 mb-8 max-w-sm mx-auto">
          Scan this QR code from the Personal Cloud mobile app to automatically connect to this server on your local network.
        </p>

        <div className="flex justify-center p-4 bg-white border-2 border-dashed border-gray-200 rounded-xl inline-block mx-auto">
          <QRCodeSVG value={pairingData} size={256} />
        </div>

        <div className="mt-8 bg-gray-50 p-4 rounded-lg inline-block">
          <p className="text-sm text-gray-500 font-medium">Manual Connection IP:</p>
          <p className="text-lg font-mono font-bold text-indigo-600">{ip}:8000</p>
        </div>
      </div>
    </div>
  );
}
